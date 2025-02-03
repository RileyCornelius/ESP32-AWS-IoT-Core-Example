#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class Payload
{
private:
    String clientId;
    bool isClientIdValid;
    float humidity;
    bool isHumidityValid;
    float temperature;
    bool isTemperatureValid;

public:
    Payload()
    {
        clientId = "";
        isClientIdValid = false;
        humidity = 0;
        isHumidityValid = false;
        temperature = 0;
        isTemperatureValid = false;
    };

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