#ifndef ACUARIS_QUERY_I2C_H
#define ACUARIS_QUERY_I2C_H

#include <Arduino.h>
#include <Vector.h>

namespace acuaris
{
    namespace com_i2c
    {
        typedef Vector<byte> QueryData;
        class Query
        {
        protected:
            byte *_deviceAddress;
            byte *_memoryAddress;
            uint8_t *_amountData; 
            static void(*_callback)(QueryData *data);
        public:
            Query(byte deviceAddress, byte memoryAddress, uint8_t amountData, void(*callback)(QueryData *data)) {
                _deviceAddress = &deviceAddress;
                _memoryAddress = &memoryAddress;
                _amountData = &amountData;
                _callback = _callback;
            }
            byte *getDeviceAddress() {
                return _deviceAddress;
            }
            byte *getMemoryAddress() {
                return _memoryAddress;
            }
            uint8_t *getAoumtData() {
                return _amountData;
            }
            void executeCallback(QueryData *data){
                _callback(data);
            }
            ~Query() {}
        };
        void(*Query::_callback)(QueryData *);
    }
}

#endif