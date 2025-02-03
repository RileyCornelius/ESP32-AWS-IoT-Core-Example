#include <Arduino.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "schema/Payload.h"
#include "secret/SecretService.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);
MqttCredentialModel mqttCredential;
WifiCredentialModel wifiCredential;
CertificateCredentialModel certificateCredential;

Payload payload;
char *payloadJson;

void setup()
{
  delay(3000); // Wait for Serial Monitor to connect

  Serial.begin(115200);
  Serial.println("Starting...");
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  SecretService configService(LittleFS);
  wifiCredential = configService.getWifiCredential();
  if (wifiCredential.isEmpty())
  {
    Serial.println("Wifi credential is empty");
    return;
  }

  mqttCredential = configService.getMqttCredential();
  if (mqttCredential.isEmpty())
  {
    Serial.println("Mqtt credential is empty");
    return;
  }

  certificateCredential = configService.getCertificateCredential();
  if (certificateCredential.isEmpty())
  {
    Serial.println("Certificate credential is empty");
    return;
  }

  WiFi.begin(wifiCredential.ssid.c_str(), wifiCredential.password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Set the certificates to the client
  espClient.setCACert(certificateCredential.ca.c_str());
  espClient.setCertificate(certificateCredential.certificate.c_str());
  espClient.setPrivateKey(certificateCredential.privateKey.c_str());

  client.setServer(mqttCredential.host.c_str(), mqttCredential.port);

  while (!client.connected())
  {
    Serial.println("Connecting to AWS IoT...");

    if (client.connect(mqttCredential.clientId.c_str()))
    {
      Serial.println("Connected to AWS IoT");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

  // Setup payload with clientId
  payload.setClientId(mqttCredential.clientId, true);
}

void loop()
{
  static uint32_t sensorInterval = 10000;
  static uint32_t previousSensorMillis = millis();
  if (millis() - previousSensorMillis >= sensorInterval)
  {
    previousSensorMillis = millis();
    float humidity = random(100);
    float temperature = random(100);
    payload.setHumidity(humidity, !isnan(humidity));
    payload.setTemperature(temperature, !isnan(temperature));
    payloadJson = payload.toJson();
    Serial.println("Publish message: ");
    Serial.println(payloadJson);
    client.publish(mqttCredential.publishTopic.c_str(), payloadJson);
  }
}