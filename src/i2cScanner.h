#ifndef I2CSCANNER_H
#define I2CSCANNER_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class I2CScanner
{
public:
    I2CScanner() {}
    ~I2CScanner() {}
    void scanAddresses();
};

#endif // I2CSCANNER_H