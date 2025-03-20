#pragma once

#include <Arduino.h>
#include <FS.h>
#include "Models/CertificateCredential.h"
#include "Models/MqttCredential.h"
#include "Models/WifiCredential.h"

class CredentialManager
{
private:
    fs::FS &fileSystem;

public:
    CredentialManager(fs::FS &fileSystem) : fileSystem(fileSystem) {};

    WifiCredential getWifiCredential();
    MqttCredential getMqttCredential();
    CertificateCredential getCertificateCredential();
};