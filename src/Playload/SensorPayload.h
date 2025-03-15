#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class SensorPayload
{
private:
    char jsonStr[512];

public:
    String timestamp = "";
    String clientId = "";
    String deviceId = "";
    float humidity = 0;
    float temperature = 0;

    const char *toJson()
    {
        JsonDocument doc;
        doc["timestamp"] = timestamp;
        doc["clientId"] = clientId;
        doc["deviceId"] = deviceId;
        doc["humidity"] = humidity;
        doc["temperature"] = temperature;
        serializeJson(doc, jsonStr);
        return jsonStr;
    }

    bool fromJson(const char *json)
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }
        timestamp = doc["timestamp"].as<String>();
        clientId = doc["clientId"].as<String>();
        deviceId = doc["deviceId"].as<String>();
        humidity = doc["humidity"].as<float>();
        temperature = doc["temperature"].as<float>();
        return true;
    }

    uint32_t length()
    {
        return strlen(jsonStr);
    }
};