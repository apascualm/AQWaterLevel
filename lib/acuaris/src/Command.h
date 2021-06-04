#ifndef ACUARIS_COMMAND_I2C_H
#define ACUARIS_COMMAND_I2C_H

#include <Arduino.h>
#include <Vector.h>
namespace acuaris
{
    namespace com_i2c
    {
        typedef Vector<byte> CommandData;
        class Command
        {
        protected:
            byte *_deviceAddress;
            byte *_memoryAddress;
            CommandData *_data;
        public:
            Command(byte deviceAddress, byte memoryAddress, CommandData data)
            {
                _deviceAddress = &deviceAddress;
            }
            byte *getDeviceAddress() {
                return _deviceAddress;
            }
            byte *getMemoryAddress() {
                return _memoryAddress;
            }
            CommandData *getData() {
                return _data;
            }
            ~Command() {}
        };
    } // namespace com_i2c

}

#endif