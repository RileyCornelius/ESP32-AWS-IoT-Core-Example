#pragma once

#include <Arduino.h>

struct WifiCredential
{
    String ssid;
    String password;

    WifiCredential() : ssid(""), password("") {};
    WifiCredential(String ssid, String password) : ssid(ssid), password(password) {};

    bool isEmpty()
    {
        return ssid == "" || password == "";
    }
};