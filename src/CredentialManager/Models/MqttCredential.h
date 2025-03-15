#pragma once

#include <Arduino.h>

struct MqttCredential
{
    int port = 0;
    String host = "";
    String clientId = "";
    String publishTopic = "";
    String subscribeTopic = "";

    bool isEmpty()
    {
        return port == 0 || host == "" || clientId == "" || publishTopic == "" || subscribeTopic == "";
    }
};