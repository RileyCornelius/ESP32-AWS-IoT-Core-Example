#pragma once

#include <Arduino.h>
#include "model/CertificateCredentialModel.h"
#include "model/MqttCredentialModel.h"
#include "model/WifiCredentialModel.h"
#include <FS.h>

class SecretService
{
private:
    fs::FS &fileSystem;

public:
    SecretService(fs::FS &fileSystem) : fileSystem(fileSystem) {};

    WifiCredentialModel getWifiCredential();
    MqttCredentialModel getMqttCredential();
    CertificateCredentialModel getCertificateCredential();
};