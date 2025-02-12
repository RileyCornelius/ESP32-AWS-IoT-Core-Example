#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>

#include <Benchmark.h>
#include <Timer.h>

#include "schema/Payload.h"
#include "secret/SecretService.h"

WiFiClientSecure espClient;
MQTTClient client;

MqttCredentialModel mqttCredential;
WifiCredentialModel wifiCredential;
CertificateCredentialModel certificateCredential;

Payload payload;
char *payloadJson;
bool configLoaded = false;

void messageHandler(String &topic, String &payload)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
  Serial.println(message);
}

bool loadConfigurationSecrets()
{
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return false;
  }

  SecretService configService(LittleFS);
  wifiCredential = configService.getWifiCredential();
  if (wifiCredential.isEmpty())
  {
    Serial.println("Wifi credential is empty");
    return false;
  }

  mqttCredential = configService.getMqttCredential();
  if (mqttCredential.isEmpty())
  {
    Serial.println("Mqtt credential is empty");
    return false;
  }

  certificateCredential = configService.getCertificateCredential();
  if (certificateCredential.isEmpty())
  {
    Serial.println("Certificate credential is empty");
    return false;
  }
  return true;
}

void connectWiFi()
{
  Serial.print("WiFi Connecting..");
  WiFi.begin(wifiCredential.ssid.c_str(), wifiCredential.password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void connectAWS()
{
  // Set the certificates to the client
  espClient.setCACert(certificateCredential.ca.c_str());
  espClient.setCertificate(certificateCredential.certificate.c_str());
  espClient.setPrivateKey(certificateCredential.privateKey.c_str());

  // Set the server details and callback
  client.begin(mqttCredential.host.c_str(), mqttCredential.port, espClient);
  client.onMessage(messageHandler);

  while (!client.connected())
  {
    Serial.println("AWS IoT Connecting...");

    if (client.connect(mqttCredential.clientId.c_str()))
    {
      Serial.println("AWS IoT Connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.lastError());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

  // Subscribe to topic
  if (!client.subscribe(mqttCredential.subscribeTopic))
  {
    Serial.println("Subscribe to topic failed");
  }

  // Setup payload with clientId
  payload.setClientId(mqttCredential.clientId, true);
}

void setup()
{
  delay(3000); // Wait for Serial Monitor to connect
  Serial.begin(115200);

  configLoaded = loadConfigurationSecrets();
  if (!configLoaded)
  {
    Serial.println("Failed to load configuration secrets");
    return;
  }

  connectWiFi();
  connectAWS();
}

void loop()
{
  if (!configLoaded)
  {
    return;
  }

  static Timer timer(10000);
  if (timer.ready())
  {
    float humidity = random(100);
    float temperature = random(100);
    payload.setHumidity(humidity, !isnan(humidity));
    payload.setTemperature(temperature, !isnan(temperature));
    payloadJson = payload.toJson();
    Serial.println("Publish message: ");
    Serial.println(payloadJson);
    BENCHMARK_MICROS_BEGIN(PUB);
    client.publish(mqttCredential.publishTopic.c_str(), payloadJson, (int)strlen(payloadJson), false, 0); // qos=0 - 1.0 ms
    // client.publish(mqttCredential.publishTopic.c_str(), payloadJson, (int)strlen(payloadJson), false, 1); // qos=1 - 100 ms
    BENCHMARK_MICROS_END(PUB);
  }

  client.loop();
}