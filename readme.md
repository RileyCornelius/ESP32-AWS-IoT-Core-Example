# AWS IoT Core

## Download SSL Certificates:

Follow the Link below to download certifications from AWS Iot Core:
https://scribehow.com/shared/Create_and_Configure_an_IoT_Device_on_AWS__if9Rh2N-Qj22QGItBz53Gw

Once you have download the three essential files for your thing:
    - The Amazon Root CA 1 (`AmazonRootCA1.pem`) file.
    - A certificate file (`blablabla-certificate.pem.crt`).
    - A private key file (`blablabla-private.pem.key`).
- Make sure to save these files securely, as they are vital for your ESP32 to communicate securely with AWS IoT Core.

The file names are too long so change them to the following:

- `blalblablabla-certificate.pem.crt` to `certificate.pem.crt`
- `blalblablabla-private.pem.key` to `private.pem.key`
- `AmazonRootCA1.pem` to `AmazonRootCA1.pem` - no need to change

After changing the name of the files We have 3 files to use in our project:

- `certificate.pem.crt`
- `private.pem.key`
- `AmazonRootCA1.pem`

## Configuring ESP32 Filesystem for AWS IoT Core Connection

In this part, We're going to configure the ESP32 filesystem to connect to AWS IoT Core. We'll use the LittleFS file system to store the SSL certificates and the AWS IoT Core endpoint.

In the root directory of your project, there is a folder called `data`. This folder will contain all the files we need to store in the ESP32 filesystem.

**Note**: The `data` folder must be in the root directory of your project. Otherwise, the ESP32 will not be able to find the files.

Copy the renamed credential files into` /data/certs`

[![data folder](https://imgur.com/LDk00Aj.png)

#### Create MQTT and WiFi Configs

The mqtt_config.json file will contain the AWS IoT Core endpoint, port, clientId, and topic to publish data.

The wifi_config.json file will contain wifi credentials.

To get host information We need to go to the AWS IoT Core console and click on the `Connect`tab then `Domain configration`. Then We can see the endpoint information under `Domain name`.

[![AWS IoT Core](https://imgur.com/LMyWdaD.png)

- **`port`** is 8883 which is used for secure connection.
- **`host`** is you AWS IoT Core endpoint (Domain name).
- **`clientId`** is the name of the *thing* we created in AWS IoT Core.
- **`publishTopic`** is the topic name that we will publish data to.
- `subscribeTopic` is the topic name that we will subscribe to.

Create mqtt_config.json file in `/data/config` with the following:

```json
{
  "port": 8883,
  "host": "YOUR_AWS_IOT_CORE_ENDPOINT",
  "clientId": "esp_test1",
  "publishTopic": "esp32/sensor/pub",
  "subscribeTopic": "esp32/sensor/sub"
}
```

Create wifi_config.json file in `/data/config` with the following:

```json
{
  "ssid": "your_wifi_name",
  "password": "your_wifi_password"
}
```

## Build and Upload Configs

Press the PlatformIO extension on right. Open `PROJECT TASKS` then `esp32xx` then `Platform`

[![Esp menu](https://imgur.com/qZmSmVl.png)

To build Filesystem Image We need to run Build Filesystem Image task:

[![Build FileSystem Image](https://media2.dev.to/dynamic/image/width=800%2Cheight=%2Cfit=scale-down%2Cgravity=auto%2Cformat=auto/https%3A%2F%2Fdev-to-uploads.s3.amazonaws.com%2Fuploads%2Farticles%2F7jc1uuersr3xlolzp006.png)](https://media2.dev.to/dynamic/image/width=800%2Cheight=%2Cfit=scale-down%2Cgravity=auto%2Cformat=auto/https%3A%2F%2Fdev-to-uploads.s3.amazonaws.com%2Fuploads%2Farticles%2F7jc1uuersr3xlolzp006.png)

After the Clicking the Build Filesystem Image task, We can see the following output in the terminal:

[![Build FileSystem Image Output](https://media2.dev.to/dynamic/image/width=800%2Cheight=%2Cfit=scale-down%2Cgravity=auto%2Cformat=auto/https%3A%2F%2Fdev-to-uploads.s3.amazonaws.com%2Fuploads%2Farticles%2F6fz32aefmw3v1kgqz9z1.png)](https://media2.dev.to/dynamic/image/width=800%2Cheight=%2Cfit=scale-down%2Cgravity=auto%2Cformat=auto/https%3A%2F%2Fdev-to-uploads.s3.amazonaws.com%2Fuploads%2Farticles%2F6fz32aefmw3v1kgqz9z1.png)
Now You can upload the filesystem image to the esp32. To do this, We need to run the Upload Filesystem Image task:

The output of the Upload Filesystem Image task is as follows:

[![Upload FileSystem Image Output](https://media2.dev.to/dynamic/image/width=800%2Cheight=%2Cfit=scale-down%2Cgravity=auto%2Cformat=auto/https%3A%2F%2Fdev-to-uploads.s3.amazonaws.com%2Fuploads%2Farticles%2Fibgh3pec3yk5j3foqgc9.png)](https://media2.dev.to/dynamic/image/width=800%2Cheight=%2Cfit=scale-down%2Cgravity=auto%2Cformat=auto/https%3A%2F%2Fdev-to-uploads.s3.amazonaws.com%2Fuploads%2Farticles%2Fibgh3pec3yk5j3foqgc9.png)
Now run `Upload and Monitor`, to upload the firmware and view the serial monitor. 

It should look look like this if everything worked correctly.

![Serial Prints](https://i.imgur.com/r4he73D.png)
