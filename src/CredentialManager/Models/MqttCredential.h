#pragma once

#include <Arduino.h>

struct MqttCredential
{
    int port;
    String host;
    String clientId;
    String publishTopic;
    String subscribeTopic;

    MqttCredential() : port(0), host(""), clientId(""), publishTopic(""), subscribeTopic("") {};
    MqttCredential(int port, String host, String clientId, String publishTopic, String subscribeTopic)
        : port(port), host(host), clientId(clientId), publishTopic(publishTopic), subscribeTopic(subscribeTopic) {};

    bool isEmpty()
    {
        return port == 0 || host == "" || clientId == "" || publishTopic == "" || subscribeTopic == "";
    }
};