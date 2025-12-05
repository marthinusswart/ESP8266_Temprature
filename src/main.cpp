#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

constexpr int SCL_PIN = 5;       // D1 = GPIO5
constexpr int SDA_PIN = 4;       // D1 = GPIO4
Adafruit_BMP280 bmp;             // I2C
constexpr int bmpAddress = 0x76; // BMP280 I2C address

#ifdef DEBUG
#define DBG(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)       /* Debug with no line feed */
#define DBGL(fmt, ...) Serial.printf(fmt "\n", ##__VA_ARGS__) /* Debug Line */
#else
#define DBG(fmt, ...)
#define DBGL(fmt, ...)
#endif

void scanAddresses();

void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
    delay(100);
    Serial.println("setup() entered");
#endif

    Wire.begin(SDA_PIN, SCL_PIN);

    if (!bmp.begin(bmpAddress))
    {
        DBGL("Could not find a valid BMP280 sensor, check wiring!");
        while (1)
            ;
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

    delay(10000);
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
