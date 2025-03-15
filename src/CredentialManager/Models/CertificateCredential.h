#pragma once

#include <Arduino.h>

struct CertificateCredential
{
    const char *ca;
    const char *certificate;
    const char *privateKey;

    CertificateCredential() : ca(""), certificate(""), privateKey("") {};
    CertificateCredential(const char *ca, const char *certificate, const char *privateKey) : ca(ca), certificate(certificate), privateKey(privateKey) {};

    bool isEmpty()
    {
        return strlen(ca) == 0 || strlen(certificate) == 0 || strlen(privateKey) == 0;
    }
};