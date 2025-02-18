#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class CalibrationPayload
{
private:
    char jsonStr[512];

public:
    String timestamp = "";
    String userId = "";
    String deviceId = "";
    float calibration = 0;

    const char *toJson()
    {
        JsonDocument doc;
        doc["timestamp"] = timestamp;
        doc["userId"] = userId;
        doc["deviceId"] = deviceId;
        doc["calibration"] = calibration;
        serializeJson(doc, jsonStr);
        return jsonStr;
    }

    uint32_t length()
    {
        return strlen(jsonStr);
    }
};