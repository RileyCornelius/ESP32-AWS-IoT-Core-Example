#pragma once

#include <Arduino.h>

class CertificateCredentialModel
{
public:
    const char *ca;
    const char *certificate;
    const char *privateKey;

    CertificateCredentialModel() : ca(""), certificate(""), privateKey("") {};
    CertificateCredentialModel(const char *ca, const char *certificate, const char *privateKey) : ca(ca), certificate(certificate), privateKey(privateKey) {};

    bool isEmpty()
    {
        return strlen(ca) == 0 || strlen(certificate) == 0 || strlen(privateKey) == 0;
    }
};