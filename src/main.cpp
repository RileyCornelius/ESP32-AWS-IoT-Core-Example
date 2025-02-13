#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <WiFiManager.h>
#include <ESP32Time.h>

#include <Benchmark.h>
#include <Timer.h>

#include "schema/Payload.h"
#include "secret/SecretService.h"

#define MQTT_MAX_PACKET_SIZE 512

WiFiClientSecure espClient;
MQTTClient mqttClient(MQTT_MAX_PACKET_SIZE);
WiFiManager wifiManager;
ESP32Time timeService;

MqttCredentialModel mqttCredential;
WifiCredentialModel wifiCredential;
CertificateCredentialModel certificateCredential;
SecretService configService(LittleFS);

bool loadAwsCredentials()
{
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

bool loadWifiCredentials()
{
  wifiCredential = configService.getWifiCredential();
  if (wifiCredential.isEmpty())
  {
    Serial.println("Wifi credential is empty");
    return false;
  }
  return true;
}

void configTimeNtp()
{
  long timezone = -6;      // CST (utc+) TZ in hours
  uint8_t daysavetime = 0; // Is daylight saving time in effect? 1 = Yes, 0 = No
  Serial.println("Time server connecting...");
  configTime(timezone * 3600, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  tm timeinfo;
  getLocalTime(&timeinfo);
  timeService.setTimeStruct(timeinfo);
  Serial.printf("Time: %s\n", timeService.getDateTime().c_str());
}

void connectWiFiManager()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // wifiManager.setConfigPortalTimeout(60);
  // wifiManager.setConfigPortalBlocking(false);
  // wifiManager.resetSettings(); // uncomment to reset saved wifi ssid and password

  Serial.print("WiFi Connecting...");
  const char *STA_WIFI_NAME = "WifiManagerAP";
  bool connected = wifiManager.autoConnect(STA_WIFI_NAME); // go to 192.168.4.1 once connected
  if (connected)
  {
    Serial.println("WiFi connected");
  }
  else
  {
    Serial.println("Wifi failed to connect");
  }
}

void connectWiFiManual()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFi.begin(wifiCredential.ssid.c_str(), wifiCredential.password.c_str());
  Serial.print("WiFi connecting..");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
}

void messageHandler(String &topic, String &payload)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
  Serial.println(message);
}

void connectAwsMqtt()
{
  // Set the certificates to the client
  espClient.setCACert(certificateCredential.ca.c_str());
  espClient.setCertificate(certificateCredential.certificate.c_str());
  espClient.setPrivateKey(certificateCredential.privateKey.c_str());

  // Set the server details and callback
  mqttClient.begin(mqttCredential.host.c_str(), mqttCredential.port, espClient);
  mqttClient.onMessage(messageHandler);

  // Connect to the MQTT broker
  uint8_t retries = 0;
  while (!mqttClient.connected() && retries < 3)
  {
    Serial.println("AWS IoT Connecting...");

    if (mqttClient.connect(mqttCredential.clientId.c_str()))
    {
      Serial.println("AWS IoT Connected");
    }
    else
    {
      retries++;
      Serial.print("failed, rc=");
      Serial.print(mqttClient.lastError());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

  // Subscribe to topic
  if (!mqttClient.subscribe(mqttCredential.subscribeTopic))
  {
    Serial.println("Subscribe to topic failed");
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(3000); // Wait for Serial Monitor to connect

  Serial.println("--------------------------------");
  Serial.printf("SDK Version: %s\n", ESP.getSdkVersion());
  Serial.printf("Free Heap: %d\n", ESP.getFreeHeap());
  Serial.printf("Flash Chip Size: %d\n", ESP.getFlashChipSize());
  Serial.printf("Sketch Size: %d\n", ESP.getSketchSize());
  Serial.printf("Free Sketch Space: %d\n", ESP.getFreeSketchSpace());
  Serial.println("--------------------------------");

  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
  }

  bool awsCredentialsLoaded = loadAwsCredentials();
  bool wifiCredentialsLoaded = loadWifiCredentials();

  if (wifiCredentialsLoaded)
  {
    connectWiFiManual();
  }
  else
  {
    connectWiFiManager();
  }

  if (awsCredentialsLoaded && WiFi.isConnected())
  {
    connectAwsMqtt();
  }

  if (WiFi.isConnected())
  {
    configTimeNtp();
  }
}

void loop()
{
  if (!WiFi.isConnected() || !mqttClient.connected())
  {
    return;
  }

  static Timer timer(10000);
  if (timer.ready())
  {
    Payload payload;
    payload.timestamp = timeService.getDateTime();
    payload.clientId = mqttCredential.clientId;
    payload.deviceId = WiFi.macAddress();
    payload.humidity = random(100);
    payload.temperature = random(100);
    const char *payloadJson = payload.toJson();
    Serial.println("Publish message: ");
    Serial.println(payloadJson);

    BENCHMARK_MICROS_BEGIN(PUB);
    mqttClient.publish(mqttCredential.publishTopic.c_str(), payloadJson, strlen(payloadJson)); // qos=0 - 1.0 ms
    // mqttClient.publish(mqttCredential.publishTopic.c_str(), payloadJson, strlen(payloadJson), false, 1); // qos=1 - 100 ms
    BENCHMARK_MICROS_END(PUB);
  }

  mqttClient.loop();
}