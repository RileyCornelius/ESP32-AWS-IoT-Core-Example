#include "CredentialManager.h"
#include <ArduinoJson.h>
#include <FS.h>

// Path inside "data" folder
#define WIFI_CONFIG_FILE "/wifi_config.json"
#define MQTT_CONFIG_FILE "/mqtt_config.json"

// extern is needed or else the linker will not find the symbols
extern const char aws_root_ca_pem_start[] asm("_binary_certs_AmazonRootCA1_pem_start");
extern const char certificate_pem_crt_start[] asm("_binary_certs_certificate_pem_crt_start");
extern const char private_pem_key_start[] asm("_binary_certs_private_pem_key_start");

String CredentialManager::readFile(const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);
    File file = fileSystem.open(path, FILE_READ);
    if (!file)
    {
        Serial.printf("Failed to open file: %s\r\n", path);
        return "";
    }
    String content = file.readString();
    file.close();
    return content;
}

CertificateCredential CredentialManager::getCertificateCredential()
{
    CertificateCredential certificateCredential = {
        .ca = aws_root_ca_pem_start,
        .certificate = certificate_pem_crt_start,
        .privateKey = private_pem_key_start,
    };
    return certificateCredential;
}

WifiCredential CredentialManager::getWifiCredential()
{
    Serial.printf("Reading file: %s\r\n", WIFI_CONFIG_FILE);
    File wifiConfigFile = fileSystem.open(WIFI_CONFIG_FILE, "r");
    if (!wifiConfigFile)
    {
        Serial.println("Failed to open wifi_config.json file");
        return WifiCredential();
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, wifiConfigFile);
    wifiConfigFile.close();
    if (error)
    {
        Serial.println("Failed to read file, using default configuration");
        return WifiCredential();
    }
    WifiCredential wifiCredential = {
        .ssid = doc["ssid"],
        .password = doc["password"],
    };
    return wifiCredential;
}

MqttCredential CredentialManager::getMqttCredential()
{
    Serial.printf("Reading file: %s\r\n", MQTT_CONFIG_FILE);
    File mqttConfigFile = fileSystem.open(MQTT_CONFIG_FILE, "r");
    if (!mqttConfigFile)
    {
        Serial.println("Failed to open mqtt_config.json file");
        return MqttCredential();
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, mqttConfigFile);
    mqttConfigFile.close();
    if (error)
    {
        Serial.println("Failed to read file, using default configuration");
        return MqttCredential();
    }
    MqttCredential mqttCredential = {
        .port = doc["port"],
        .host = doc["host"],
        .clientId = doc["clientId"],
        .publishTopic = doc["publishTopic"],
        .subscribeTopic = doc["subscribeTopic"],
    };
    return mqttCredential;
}