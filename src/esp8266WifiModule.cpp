#include "esp8266WifiModule.h"
#include "logger.h"
#include "esp8266EEPROM.h"
#include <ESP8266WiFi.h>       // Add WiFi library
#include <ESP8266HTTPClient.h> // Add HTTP client library
#include <Wire.h>
#include <SPI.h>

// Function to connect to WiFi
void ESP8266WiFiModule::connectToWiFi(const String &ssid, const String &password)
{
    DBGL("Connecting to WiFi SSID: %s", ssid.c_str());

    this->password = password;
    this->ssid = ssid;
    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for connection with timeout
    int timeout = 20; // 10 seconds (20 * 500ms)
    while (WiFi.status() != WL_CONNECTED && timeout > 0)
    {
        delay(500);
        Serial.print(".");
        timeout--;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        DBGL("WiFi connected");
        DBGL("IP address: %s", WiFi.localIP().toString().c_str());
        wifiConfigured = true;
    }
    else
    {
        DBGL("Failed to connect to WiFi");
    }
}

void ESP8266WiFiModule::reconnectToWiFi()
{
    if (wifiConfigured)
    {
        connectToWiFi(ssid, password);
    }
}

// Function to configure WiFi via serial input
void ESP8266WiFiModule::configureWiFi()
{
    DBGL("=== WiFi Configuration ===");

    ESP8266EEPROM eeprom; // EEPROM handler

    // Get SSID
    DBGL("Enter WiFi SSID:");
    while (!Serial.available())
    {
        yield(); // Allow ESP8266 to handle background tasks
    }

    ssid = Serial.readStringUntil('\n');
    ssid.trim(); // Remove any whitespace/newlines
    DBGL("SSID received: %s", ssid.c_str());

    // Get password
    DBGL("Enter WiFi password:");
    while (!Serial.available())
    {
        yield();
    }

    password = Serial.readStringUntil('\n');
    password.trim();
    DBGL("Password received (not shown for security)");

    wifiConfigured = true;

    // Save credentials to EEPROM
    eeprom.saveCredentialsToEEPROM(ssid, password);

    DBGL("WiFi configuration complete");
}

void ESP8266WiFiModule::sleepWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        DBGL("Putting WiFi to sleep mode");
        WiFi.forceSleepBegin();
    }
}

void ESP8266WiFiModule::wakeWiFi()
{
    if (wifiConfigured)
    {
        DBGL("Waking up WiFi");
        WiFi.forceSleepWake();
        yield(); // Allow ESP8266 to handle background tasks
    }
}

bool ESP8266WiFiModule::isWiFiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

