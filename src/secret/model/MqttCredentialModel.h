#pragma once

#include <Arduino.h>

class MqttCredentialModel
{
public:
    int port;
    String host;
    String clientId;
    String publishTopic;

    MqttCredentialModel() : port(0), host(""), clientId(""), publishTopic("") {};
    MqttCredentialModel(int port, String host, String clientId, String publishTopic) : port(port), host(host), clientId(clientId), publishTopic(publishTopic) {};

    bool isEmpty()
    {
        return port == 0 || host == "" || clientId == "" || publishTopic == "";
    }
};