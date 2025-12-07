#ifndef ESP8266WIFIMODULE_H
#define ESP8266WIFIMODULE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>       // Add WiFi library
#include <ESP8266HTTPClient.h> // Add HTTP client library

class ESP8266WiFiModule
{
public:
    ESP8266WiFiModule() {}
    ~ESP8266WiFiModule() {}
    void connectToWiFi(const String &ssid, const String &password);
    void reconnectToWiFi();
    void configureWiFi();
    void sleepWiFi();
    void wakeWiFi();
    bool isWiFiConnected();

private:
    String ssid = "";
    String password = "";
    bool wifiConfigured = false;
};

#endif // ESP8266WIFIMODULE_H