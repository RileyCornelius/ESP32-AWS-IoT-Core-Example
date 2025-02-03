#include <Arduino.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "secret/SecretService.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);
MqttCredentialModel mqttCredential;
WifiCredentialModel wifiCredential;
CertificateCredentialModel certificateCredential;

void setup()
{
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
}

void loop() {}

// #include "secrets.h"
// #include <WiFiClientSecure.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include "WiFi.h"

// #define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
// #define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// struct Data
// {
//   uint32_t temperature;
//   uint32_t humidity;
// };

// WiFiClientSecure net = WiFiClientSecure();
// PubSubClient client(net);

// Data generateRandomData()
// {
//   return Data{
//       .temperature = (uint32_t)random(100),
//       .humidity = (uint32_t)random(100),
//   };
// }

// void connectAWS()
// {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//   Serial.println("Connecting to Wi-Fi");

//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");
//   }

//   // Configure WiFiClientSecure to use the AWS IoT device credentials
//   net.setCACert(AWS_CERT_CA);
//   net.setCertificate(AWS_CERT_CRT);
//   net.setPrivateKey(AWS_CERT_PRIVATE);

//   // Connect to the MQTT broker on the AWS endpoint we defined earlier
//   client.setServer(AWS_IOT_ENDPOINT, 8883);

//   // Create a message handler
//   client.setCallback(messageHandler);

//   Serial.println("Connecting to AWS IOT");

//   while (!client.connect(THINGNAME))
//   {
//     Serial.print(".");
//     delay(100);
//   }

//   if (!client.connected())
//   {
//     Serial.println("AWS IoT Timeout!");
//     return;
//   }

//   // Subscribe to a topic
//   client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

//   Serial.println("AWS IoT Connected!");
// }

// void publishDataMessage(Data &data)
// {
//   StaticJsonDocument<200> doc;
//   doc["humidity"] = data.humidity;
//   doc["temperature"] = data.temperature;
//   char jsonBuffer[512];
//   serializeJson(doc, jsonBuffer); // print to client

//   client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
// }

// void messageHandler(char *topic, byte *payload, unsigned int length)
// {
//   Serial.print("incoming: ");
//   Serial.println(topic);

//   StaticJsonDocument<200> doc;
//   deserializeJson(doc, payload);
//   const char *message = doc["message"];
//   Serial.println(message);
// }

// void setup()
// {
//   Serial.begin(115200);
//   connectAWS();
//   randomSeed(analogRead(0)); // Initialize the random data generator
// }

// void loop()
// {
//   Data data = generateRandomData();

//   if (isnan(data.humidity) || isnan(data.temperature)) // Check if any reads failed and exit early (to try again).
//   {
//     Serial.println(F("Data read failed!"));
//     return;
//   }

//   Serial.print(F("Humidity: "));
//   Serial.print(data.humidity);
//   Serial.print(F("%  Temperature: "));
//   Serial.print(data.temperature);
//   Serial.println(F("°C "));

//   publishDataMessage(data);
//   client.loop();
//   delay(1000);
// }