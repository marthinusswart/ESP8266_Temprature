#include "esp8266EEPROM.h"
#include "logger.h"

// Function to save WiFi credentials to EEPROM
void ESP8266EEPROM::saveCredentialsToEEPROM(const String &ssid, const String &password)
{
    DBGL("Saving WiFi credentials to EEPROM");

    // Write magic number to indicate valid data
    EEPROM.put(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);

    // Write SSID (with bounds checking)
    uint8_t ssidLength = min(ssid.length(), (unsigned int)MAX_SSID_LENGTH - 1);
    for (uint8_t i = 0; i < ssidLength; i++)
    {
        EEPROM.write(EEPROM_SSID_ADDR + i, ssid[i]);
    }
    EEPROM.write(EEPROM_SSID_ADDR + ssidLength, 0); // Null terminator

    // Write password (with bounds checking)
    uint8_t passLength = min(password.length(), (unsigned int)MAX_PASS_LENGTH - 1);
    for (uint8_t i = 0; i < passLength; i++)
    {
        EEPROM.write(EEPROM_PASS_ADDR + i, password[i]);
    }
    EEPROM.write(EEPROM_PASS_ADDR + passLength, 0); // Null terminator

    // Commit changes to flash
    bool success = EEPROM.commit();
    if (success)
    {
        DBGL("Credentials saved successfully");
    }
    else
    {
        DBGL("Failed to save credentials");
    }
}

// Function to load WiFi credentials from EEPROM
bool ESP8266EEPROM::loadCredentialsFromEEPROM(String &ssid, String &password)
{
    // Check magic number to validate data
    uint16_t magic;
    EEPROM.get(EEPROM_MAGIC_ADDR, magic);

    if (magic != EEPROM_MAGIC_VALUE)
    {
        DBGL("No valid WiFi credentials found in EEPROM");
        return false;
    }

    // Read SSID
    char ssidBuffer[MAX_SSID_LENGTH];
    for (int i = 0; i < MAX_SSID_LENGTH - 1; i++)
    {
        ssidBuffer[i] = EEPROM.read(EEPROM_SSID_ADDR + i);
        if (ssidBuffer[i] == 0)
            break; // Stop at null terminator
    }
    ssidBuffer[MAX_SSID_LENGTH - 1] = 0; // Ensure null termination

    // Read password
    char passBuffer[MAX_PASS_LENGTH];
    for (int i = 0; i < MAX_PASS_LENGTH - 1; i++)
    {
        passBuffer[i] = EEPROM.read(EEPROM_PASS_ADDR + i);
        if (passBuffer[i] == 0)
            break; // Stop at null terminator
    }
    passBuffer[MAX_PASS_LENGTH - 1] = 0; // Ensure null termination

    // Update the passed reference variables
    ssid = String(ssidBuffer);
    password = String(passBuffer);

    DBGL("Loaded SSID from EEPROM: %s", ssid.c_str());

    return true;
}

ESP8266EEPROM::ESP8266EEPROM()
{
    // Initialize EEPROM
    EEPROM.begin(EEPROM_SIZE);
}