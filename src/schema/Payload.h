#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class Payload
{
private:
    char jsonStr[512];
    uint32_t jsonLength = 0;

public:
    String clientId = "";
    String deviceId = "";
    float humidity = 0;
    float temperature = 0;

    const char *toJson()
    {
        DynamicJsonDocument doc(256);
        doc["clientId"] = clientId;
        doc["deviceId"] = deviceId;
        doc["humidity"] = humidity;
        doc["temperature"] = temperature;
        serializeJson(doc, jsonStr, sizeof(jsonStr));
        jsonLength = strlen(jsonStr);
        return jsonStr;
    }

    uint32_t length()
    {
        assert(jsonLength > 0);
        return jsonLength;
    }
};