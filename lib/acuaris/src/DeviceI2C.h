#ifndef ACUARIS_DEVICE_I2C_H
#define ACUARIS_DEVICE_I2C_H

#include <Arduino.h>
#include <Vector.h>
#include <Wire.h>
namespace acuaris
{
    namespace com_i2c
    {
        class DeviceI2C
        {
        private:
            /* data */
        protected:
            TwoWire *i2c;

        public:
            DeviceI2C()
            {
                i2c = &Wire;
            };
            ~DeviceI2C(){};
            virtual void begin() = 0;
        };
    }
}

#endif