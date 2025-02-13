#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class Payload
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
        DynamicJsonDocument doc(512);
        doc["timestamp"] = timestamp;
        doc["clientId"] = clientId;
        doc["deviceId"] = deviceId;
        doc["humidity"] = humidity;
        doc["temperature"] = temperature;
        serializeJson(doc, jsonStr, sizeof(jsonStr));
        return jsonStr;
    }

    uint32_t length()
    {
        return strlen(jsonStr);
    }
};