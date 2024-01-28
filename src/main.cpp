#define LED_PIN 2
#define DHT_PIN 15
#define DHT_TYPE DHT11

unsigned long previousSensorMillis = 0;
const long sensorInterval = 10000;

#include <Arduino.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "../service/ConfigService.h"
#include "../model/PayloadModel.h"
#include <DHT.h>
WiFiClientSecure espClient;
PubSubClient client(espClient);
MqttCredentialModel mqttCredential;
WifiCredentialModel wifiCredential;
CertificateCredentialModel certificateCredential;
PayloadModel payloadModel;
char *payload;
DHT dht(DHT_PIN, DHT_TYPE);

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting...");
    if (!LittleFS.begin())
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    ConfigService configService(LittleFS);
    wifiCredential = configService.getWifiCredential();
    if (wifiCredential.isEmpty())
    {
        Serial.println("Wifi credential is empty");
        return;
    }

    mqttCredential = configService.getMqttCredential();
    if (mqttCredential.isEmpty())
    {
        Serial.println("Mqtt credential is empty");
        return;
    }

    certificateCredential = configService.getCertificateCredential();
    if (certificateCredential.isEmpty())
    {
        Serial.println("Certificate credential is empty");
        return;
    }

    WiFi.begin(wifiCredential.ssid.c_str(), wifiCredential.password.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        delay(50);
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, HIGH);
        delay(1000);
        Serial.print(".");
    }
    Serial.println("WiFi connected");

    // Set the certificates to the client
    espClient.setCACert(certificateCredential.ca.c_str());
    espClient.setCertificate(certificateCredential.certificate.c_str());
    espClient.setPrivateKey(certificateCredential.privateKey.c_str());

    client.setServer(mqttCredential.host.c_str(), mqttCredential.port);

    while (!client.connected())
    {
        Serial.println("Connecting to AWS IoT...");

        if (client.connect(mqttCredential.clientId.c_str()))
        {
            Serial.println("Connected to AWS IoT");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
    // set the payload model
    payloadModel = PayloadModel();
    payloadModel.setClientId(mqttCredential.clientId, true);
}
void loop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousSensorMillis >= sensorInterval)
    {
        digitalWrite(LED_PIN, HIGH);
        previousSensorMillis = currentMillis;
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();
        payloadModel.setHumidity(humidity, !isnan(humidity));
        payloadModel.setTemperature(temperature, !isnan(temperature));
        payload = payloadModel.toJson();
        Serial.println("Publish message: ");
        Serial.println(payload);
        client.publish(mqttCredential.publishTopic.c_str(), payload);
    }
    digitalWrite(LED_PIN, LOW);
}