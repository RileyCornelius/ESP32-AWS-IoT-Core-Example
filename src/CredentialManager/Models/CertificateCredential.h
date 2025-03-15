#pragma once

#include <Arduino.h>

struct CertificateCredential
{
    const char *ca = 0;
    const char *certificate = 0;
    const char *privateKey = 0;

    bool isEmpty()
    {
        return ca == 0 || certificate == 0 || privateKey == 0;
    }
};