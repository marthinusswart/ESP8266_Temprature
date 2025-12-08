#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "esp8266EEPROM.h"
#include "logger.h"
#include "esp8266WifiModule.h"
#include "i2cScanner.h"
#include "esp8266RESTModule.h"

constexpr int SCL_PIN = 5;       // D1 = GPIO5
constexpr int SDA_PIN = 4;       // D1 = GPIO4
Adafruit_BMP280 bmp;             // I2C
constexpr int bmpAddress = 0x76; // BMP280 I2C address
ESP8266EEPROM eeprom;            // EEPROM handler
ESP8266WiFiModule wifiModule;    // WiFi module handler
I2CScanner i2cScanner;           // I2C scanner handler
ESP8266RESTModule restModule;    // REST module handler

// WiFi credentials (volatile memory)
String ssid = "";
String password = "";
bool wifiConfigured = false;
const String componmentId = "esp8266_bmp280_01";

void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
    delay(100);
    Serial.println("setup() entered");
#endif

    Wire.begin(SDA_PIN, SCL_PIN);

    i2cScanner.scanAddresses();

    // Try to load WiFi credentials from EEPROM
    if (eeprom.loadCredentialsFromEEPROM(ssid, password))
    {
        wifiConfigured = true;
        DBGL("WiFi credentials loaded from EEPROM");
    }

    if (!bmp.begin(bmpAddress))
    {
        DBGL("Could not find a valid BMP280 sensor, check wiring!");
        while (1)
            ;
    }

    // Prompt user to configure WiFi
    DBGL("Would you like to configure WiFi? (y/n)");

    // Wait for user input with a timeout
    unsigned long startTime = millis();
    while (millis() - startTime < 2000)
    { // 2 second timeout
        if (Serial.available() > 0)
        {
            char response = Serial.read();
            if (response == 'y' || response == 'Y')
            {
                wifiModule.configureWiFi();
                break;
            }
            else if (response == 'n' || response == 'N')
            {
                DBGL("WiFi configuration skipped");
                break;
            }
        }
        yield(); // Allow ESP8266 to handle background tasks
    }

    // If WiFi is configured, connect
    if (wifiConfigured)
    {
        wifiModule.connectToWiFi(ssid, password);

        // Check if API endpoint is alive
        if (wifiModule.isWiFiConnected())
        {
            DBGL("Checking if API endpoint is alive...");
            bool apiAlive = restModule.isAlive();
            if (apiAlive)
            {
                DBGL("API endpoint is alive!");
            }
            else
            {
                DBGL("API endpoint is not responding!");
            }
        }
    }
}

void loop()
{
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure() / 1000.0); // convert Pa to kPa
    Serial.println(" kPa");

    Serial.print("Approx altitude = ");
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");

    bool apiPost = restModule.updateTemperatureReading(componmentId, String(bmp.readTemperature()));
    if (apiPost)
    {
        DBGL("API endpoint posted!");
    }
    else
    {
        DBGL("API endpoint is not responding!");
    }

    // Put WiFi into modem sleep mode to save power before delay
    if (!wifiModule.isWiFiConnected())
    {
        wifiModule.reconnectToWiFi();
    }

    // delay(60 * 1000); // wait 60 seconds
    delay(60 * 1000); // wait 10 seconds

    // Wake up WiFi after delay if it was previously connected
    // if (wifiConfigured)
    // {
    //     wifiModule.wakeWiFi();
    //     // Only attempt to reconnect if it was previously connected
    //     if (!wifiModule.isWiFiConnected())
    //     {
    //         wifiModule.reconnectToWiFi();
    //     }
    // }
}