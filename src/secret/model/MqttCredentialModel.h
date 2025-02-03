#pragma once

#include <Arduino.h>

class MqttCredentialModel
{
public:
    int port;
    String host;
    String clientId;
    String publishTopic;
    String subscribeTopic;

    MqttCredentialModel() : port(0), host(""), clientId(""), publishTopic(""), subscribeTopic("") {};
    MqttCredentialModel(int port, String host, String clientId, String publishTopic, String subscribeTopic)
        : port(port), host(host), clientId(clientId), publishTopic(publishTopic), subscribeTopic(subscribeTopic) {};

    bool isEmpty()
    {
        return port == 0 || host == "" || clientId == "" || publishTopic == "" || subscribeTopic == "";
    }
};