#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class Payload
{
private:
    String clientId = "";
    bool isClientIdValid = false;
    float humidity = 0;
    bool isHumidityValid = false;
    float temperature = 0;
    bool isTemperatureValid = false;

public:
    void setClientId(String clientId, bool isClientIdValid)
    {
        this->clientId = clientId;
        this->isClientIdValid = isClientIdValid;
    };

    void setHumidity(float humidity, bool isHumidityValid)
    {
        this->humidity = humidity;
        this->isHumidityValid = isHumidityValid;
    };

    void setTemperature(float temperature, bool isTemperatureValid)
    {
        this->temperature = temperature;
        this->isTemperatureValid = isTemperatureValid;
    };

    char *toJson()
    {
        static char buffer[512];
        DynamicJsonDocument doc(256);
        if (isClientIdValid)
        {
            doc["clientId"] = clientId;
        }
        else
        {
            doc["clientId"] = nullptr;
        }
        if (isHumidityValid)
        {
            doc["humidity"] = humidity;
        }
        else
        {
            doc["humidity"] = nullptr;
        }
        if (isTemperatureValid)
        {
            doc["temperature"] = temperature;
        }
        else
        {
            doc["temperature"] = nullptr;
        }

        serializeJson(doc, buffer);
        return buffer;
    };
};