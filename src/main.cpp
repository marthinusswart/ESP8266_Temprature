#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h> // Add WiFi library
#include <EEPROM.h>      // Add EEPROM library

constexpr int SCL_PIN = 5;       // D1 = GPIO5
constexpr int SDA_PIN = 4;       // D1 = GPIO4
Adafruit_BMP280 bmp;             // I2C
constexpr int bmpAddress = 0x76; // BMP280 I2C address

// EEPROM configuration
constexpr int EEPROM_SIZE = 512;           // Size of EEPROM to initialize
constexpr int EEPROM_MAGIC_ADDR = 0;       // Address for magic number
constexpr int EEPROM_MAGIC_VALUE = 0xAB42; // Magic number to validate EEPROM data
constexpr int EEPROM_SSID_ADDR = 2;        // Start address for SSID (2 bytes after magic)
constexpr int EEPROM_PASS_ADDR = 66;       // Start address for password (64 bytes after SSID start)
constexpr int MAX_SSID_LENGTH = 32;        // Maximum SSID length (plus null terminator)
constexpr int MAX_PASS_LENGTH = 64;        // Maximum password length (plus null terminator)

// WiFi credentials (volatile memory)
String ssid = "";
String password = "";
bool wifiConfigured = false;

#ifdef DEBUG
#define DBG(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)       /* Debug with no line feed */
#define DBGL(fmt, ...) Serial.printf(fmt "\n", ##__VA_ARGS__) /* Debug Line */
#else
#define DBG(fmt, ...)
#define DBGL(fmt, ...)
#endif

void scanAddresses();
void configureWiFi();
void connectToWiFi();
void saveCredentialsToEEPROM(const String &ssid, const String &password);
bool loadCredentialsFromEEPROM();

void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
    delay(100);
    Serial.println("setup() entered");
#endif

    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize EEPROM
    EEPROM.begin(EEPROM_SIZE);

    // Try to load WiFi credentials from EEPROM
    if (loadCredentialsFromEEPROM())
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
                configureWiFi();
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
        connectToWiFi();
    }
}

void loop()
{
    // scanAddresses();

    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure() / 1000.0); // convert Pa to kPa
    Serial.println(" kPa");

    Serial.print("Approx altitude = ");
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");

    // Put WiFi into modem sleep mode to save power before delay
    if (WiFi.status() == WL_CONNECTED)
    {
        DBGL("Putting WiFi to sleep mode");
        WiFi.forceSleepBegin();
    }

    delay(60 * 1000); // wait 60 seconds

    // Wake up WiFi after delay if it was previously connected
    if (wifiConfigured)
    {
        DBGL("Waking up WiFi");
        WiFi.forceSleepWake();
        yield(); // Allow ESP8266 to handle background tasks
        // Only attempt to reconnect if it was previously connected
        if (WiFi.status() != WL_CONNECTED)
        {
            connectToWiFi();
        }
    }
}

void scanAddresses()
{
    DBGL("Scanning...");

    for (uint8_t address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();

        if (error == 0)
        {
            DBGL("I2C device found at address 0x%02X", address);
        }
        else if (error == 4)
        {
            DBGL("Unknown error at address 0x%02X", address);
        }
        else
        {
            // DBGL("No I2C device found at address 0x%02X", address);
        }
    }

    delay(5000); // wait 5 seconds for next scan
}

// Function to configure WiFi via serial input
void configureWiFi()
{
    DBGL("=== WiFi Configuration ===");

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
    saveCredentialsToEEPROM(ssid, password);

    DBGL("WiFi configuration complete");
}

// Function to connect to WiFi
void connectToWiFi()
{
    DBGL("Connecting to WiFi SSID: %s", ssid.c_str());

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
    }
    else
    {
        DBGL("Failed to connect to WiFi");
    }
}

// Function to save WiFi credentials to EEPROM
void saveCredentialsToEEPROM(const String &ssid, const String &password)
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
bool loadCredentialsFromEEPROM()
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

    // Update global variables
    ssid = String(ssidBuffer);
    password = String(passBuffer);

    DBGL("Loaded SSID from EEPROM: %s", ssid.c_str());

    return true;
}
