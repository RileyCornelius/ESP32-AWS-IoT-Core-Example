#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <WiFiManager.h>
#include <ESP32Time.h>

#include <Benchmark.h>
#include <Timer.h>

#include "schema/SensorPayload.h"
#include "schema/CalibrationPayload.h"
#include "secret/SecretService.h"

#define BREAK_LINE "----------------------------------------"
#define MQTT_MAX_PACKET_SIZE 512

WiFiClientSecure sslClient;
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

// List of timezones: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define TIMEZONE_WINNIPEG "CST6CDT,M3.2.0,M11.1.0" // America/Winnipeg timezone
#define TIMEZONE_UTC "UTC0"                        // Universal Time Coordinated timezone
#define TIME_SERVER_1 "0.ca.pool.ntp.org"
#define TIME_SERVER_2 "0.pool.ntp.org"
#define TIME_SERVER_3 "1.pool.ntp.org"
void configTimeNtp()
{
  Serial.println("Time server connecting...");
  tm timeinfo;
  configTzTime(TIMEZONE_WINNIPEG, TIME_SERVER_1, TIME_SERVER_2, TIME_SERVER_3);
  if (getLocalTime(&timeinfo, 10000)) // wait up to 10 sec to sync
  {
    Serial.println("Time server connected");
  }
  else
  {
    Serial.println("Failed to get time from server");
  }
  Serial.println(timeService.getDateTime(true));
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

  JsonDocument doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
  Serial.println(message);
}

void connectAwsMqtt()
{
  // Set the certificates to the client
  sslClient.setCACert(certificateCredential.ca.c_str());
  sslClient.setCertificate(certificateCredential.certificate.c_str());
  sslClient.setPrivateKey(certificateCredential.privateKey.c_str());

  // Set the server details and callback
  mqttClient.begin(mqttCredential.host.c_str(), mqttCredential.port, sslClient);
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

  Serial.println(BREAK_LINE);
  Serial.printf("ESP SDK Version: %s\n", ESP.getSdkVersion());
  Serial.printf("Arduino Core Version: v%d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
  Serial.printf("Sketch Size: %d\n", ESP.getSketchSize());
  Serial.printf("Free Sketch Space: %d\n", ESP.getFreeSketchSpace());
  Serial.println(BREAK_LINE);

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

  Serial.println(BREAK_LINE);
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
    static bool firstTime = true;
    if (firstTime)
    {
      firstTime = false;
      CalibrationPayload calibrationPayload;
      calibrationPayload.timestamp = timeService.getTime("%Y-%m-%d %H:%M:%S");
      calibrationPayload.userId = "TestUser";
      calibrationPayload.deviceId = WiFi.macAddress();
      calibrationPayload.calibration = random(100);

      BENCHMARK_MICROS_BEGIN(Json);
      const char *calibrationJson = calibrationPayload.toJson();
      BENCHMARK_MICROS_END(Json);
      Serial.printf("Json: %s\n", calibrationJson);

      mqttClient.publish(mqttCredential.publishTopic.c_str(), calibrationJson, strlen(calibrationJson));
    }

    static SensorPayload payload;
    payload.timestamp = timeService.getTime("%Y-%m-%d %H:%M:%S");
    payload.clientId = mqttCredential.clientId;
    payload.deviceId = WiFi.macAddress();
    payload.humidity = random(100);
    payload.temperature = random(100);
    const char *payloadJson = payload.toJson();

    Serial.printf("Publish: %s\n", payloadJson);
    BENCHMARK_MICROS_BEGIN(Publish);
    mqttClient.publish(mqttCredential.publishTopic.c_str(), payloadJson, strlen(payloadJson)); // qos=0 - 1.0 ms
    // mqttClient.publish(mqttCredential.publishTopic.c_str(), payloadJson, strlen(payloadJson), false, 1); // qos=1 - 100 ms
    BENCHMARK_MICROS_END(Publish);
  }

  mqttClient.loop();
}