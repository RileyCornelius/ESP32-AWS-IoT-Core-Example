#pragma once

#include <Arduino.h>

struct WifiCredential
{
    String ssid = "";
    String password = "";

    bool isEmpty()
    {
        return ssid == "" || password == "";
    }
};