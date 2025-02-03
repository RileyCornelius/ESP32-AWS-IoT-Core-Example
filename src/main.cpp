#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

struct Data
{
  uint32_t temperature;
  uint32_t humidity;
};

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

Data generateRandomData()
{
  return Data{
      .temperature = (uint32_t)random(100),
      .humidity = (uint32_t)random(100),
  };
}

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishDataMessage(Data &data)
{
  StaticJsonDocument<200> doc;
  doc["humidity"] = data.humidity;
  doc["temperature"] = data.temperature;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(char *topic, byte *payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
  Serial.println(message);
}

void setup()
{
  Serial.begin(115200);
  connectAWS();
  randomSeed(analogRead(0)); // Initialize the random data generator
}

void loop()
{
  Data data = generateRandomData();

  if (isnan(data.humidity) || isnan(data.temperature)) // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Data read failed!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(data.humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(data.temperature);
  Serial.println(F("°C "));

  publishDataMessage(data);
  client.loop();
  delay(1000);
}