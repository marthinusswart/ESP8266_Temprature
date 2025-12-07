#ifndef ESP8266RESTMODULE_H
#define ESP8266RESTMODULE_H
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "esp8266WifiModule.h"
#include "logger.h"

class ESP8266RESTModule
{
public:
    ESP8266RESTModule() {}
    ~ESP8266RESTModule() {}
    bool checkAPIEndpoint(const String &url);
    bool postToAPIEndpoint(const String &url, const String &payload);
    bool isAlive();
    bool updateTemperatureReading(const String componentId, const String &temperature);

private:
    // API endpoint
    const String apiEndpoint = "http://192.168.0.98:8000/api/v1/";
};
#endif // ESP8266RESTMODULE_H