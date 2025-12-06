#include "i2cScanner.h"
#include "logger.h"

void I2CScanner::scanAddresses()
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