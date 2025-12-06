#ifndef ESP8266EEPROM_H
#define ESP8266EEPROM_H

#include <Arduino.h>
#include <EEPROM.h>

// EEPROM configuration
constexpr int EEPROM_SIZE = 512;           // Size of EEPROM to initialize
constexpr int EEPROM_MAGIC_ADDR = 0;       // Address for magic number
constexpr int EEPROM_MAGIC_VALUE = 0xAB42; // Magic number to validate EEPROM data
constexpr int EEPROM_SSID_ADDR = 2;        // Start address for SSID (2 bytes after magic)
constexpr int EEPROM_PASS_ADDR = 66;       // Start address for password (64 bytes after SSID start)
constexpr int MAX_SSID_LENGTH = 32;        // Maximum SSID length (plus null terminator)
constexpr int MAX_PASS_LENGTH = 64;        // Maximum password length (plus null terminator)

class ESP8266EEPROM
{
public:
    ESP8266EEPROM();
    ~ESP8266EEPROM() {}
    void saveCredentialsToEEPROM(const String &ssid, const String &password);
    bool loadCredentialsFromEEPROM(String &ssid, String &password);

private:
};

// void saveCredentialsToEEPROM(const String &ssid, const String &password);
// bool loadCredentialsFromEEPROM(String &ssid, String &password);

#endif // ESP8266EEPROM_H