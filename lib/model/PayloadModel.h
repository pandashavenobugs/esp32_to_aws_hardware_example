// lib/model/PayloadModel.h
// PayloadModel class definition.

#ifndef PAYLOADMODEL_H
#define PAYLOADMODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class PayloadModel
{
private:
    String clientId;
    bool isClientIdValid;
    float humidity;
    bool isHumidityValid;
    float temperature;
    bool isTemperatureValid;

public:
    PayloadModel()
    {
        clientId = "";
        isClientIdValid = false;
        humidity = 0;
        isHumidityValid = false;
        temperature = 0;
        isTemperatureValid = false;
    };
    void setClientId(String clientId, bool isClientIdValid)
    {
        this->clientId = clientId;
        this->isClientIdValid = isClientIdValid;
    };
    void setHumidity(float humidity, bool isHumidityValid)
    {
        this->humidity = humidity;
        this->isHumidityValid = isHumidityValid;
    };
    void setTemperature(float temperature, bool isTemperatureValid)
    {
        this->temperature = temperature;
        this->isTemperatureValid = isTemperatureValid;
    };

    char *toJson()
    {
        static char buffer[512];
        DynamicJsonDocument doc(256);
        if (this->isClientIdValid)
        {
            doc["clientId"] = this->clientId;
        }
        else
        {
            doc["clientId"] = nullptr;
        }
        if (this->isHumidityValid)
        {
            doc["humidity"] = this->humidity;
        }
        else
        {
            doc["humidity"] = nullptr;
        }
        if (this->isTemperatureValid)
        {
            doc["temperature"] = this->temperature;
        }
        else
        {
            doc["temperature"] = nullptr;
        }

        serializeJson(doc, buffer);
        return buffer;
    };
};
#endif