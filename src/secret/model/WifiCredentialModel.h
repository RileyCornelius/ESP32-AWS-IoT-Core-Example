#pragma once

#include <Arduino.h>

class WifiCredentialModel
{
public:
    String ssid;
    String password;
    WifiCredentialModel() : ssid(""), password("") {};
    WifiCredentialModel(String ssid, String password) : ssid(ssid), password(password) {};

    bool isEmpty()
    {
        return ssid == "" || password == "";
    }
};