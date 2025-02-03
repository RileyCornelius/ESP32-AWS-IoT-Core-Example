#include "SecretService.h"
#include "SecretConstants.h"
#include <ArduinoJson.h>
#include <FS.h>

String readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path, FILE_READ);
    if (!file)
    {
        Serial.printf("Failed to open file: %s\r\n", path);
        return "";
    }
    return file.readString();
}

CertificateCredentialModel SecretService::getCertificateCredential()
{
    String ca = readFile(fileSystem, AWS_ROOT_CA_FILE);
    String certificate = readFile(fileSystem, AWS_CERT_FILE);
    String privateKey = readFile(fileSystem, AWS_PRIVATE_KEY_FILE);
    return CertificateCredentialModel(ca, certificate, privateKey);
}

WifiCredentialModel SecretService::getWifiCredential()
{
    File wifiConfigFile = fileSystem.open(WIFI_CONFIG_FILE, "r");
    if (!wifiConfigFile)
    {
        Serial.println("Failed to open wifi_config.json file");
        return WifiCredentialModel();
    }
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, wifiConfigFile);
    if (error)
    {
        Serial.println("Failed to read file, using default configuration");
        wifiConfigFile.close();
        return WifiCredentialModel();
    }
    String ssid = doc["ssid"];
    String password = doc["password"];
    wifiConfigFile.close();
    return WifiCredentialModel(ssid, password);
}

MqttCredentialModel SecretService::getMqttCredential()
{
    File mqttConfigFile = fileSystem.open(MQTT_CONFIG_FILE, "r");
    if (!mqttConfigFile)
    {
        Serial.println("Failed to open mqtt_config.json file");
        return MqttCredentialModel();
    }
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, mqttConfigFile);
    if (error)
    {
        Serial.println("Failed to read file, using default configuration");
        mqttConfigFile.close();
        return MqttCredentialModel();
    }
    String host = doc["host"];
    int port = doc["port"];
    String clientId = doc["clientId"];
    String publishTopic = doc["publishTopic"];
    String subscribeTopic = doc["subscribeTopic"];
    mqttConfigFile.close();
    return MqttCredentialModel(port, host, clientId, publishTopic, subscribeTopic);
}