#ifndef ACUARIS_SERVER_I2C_H
#define ACUARIS_SERVER_I2C_H

#include <Arduino.h>
#include <Vector.h>
#include <Wire.h>
#include <DeviceI2C.h>
#include <CommandHandlerI2C.h>
#include <QueryHandlerI2C.h>

namespace acuaris
{
    namespace com_i2c
    {

        class ServerI2C;

        class ServerI2C : public DeviceI2C
        {
        protected:
            byte _selectedAddress;
            int _lastRedirection = 0;
            uint8_t _deviceAddress;
            const int AMOUNT_DATA_BYTES;
            Commands _serverCommands;
            Queries _serverQueries;
            virtual byte getDefaultAddress()
            {
                return 0;
            };
            void setAddress(byte address)
            {
                _selectedAddress = address;
            }
            virtual size_t getMaxDataElements()
            {
                return 10;
            }
            void processCommand(CommandData data)
            {
                /* Serial.print("process Action Function ");
                Serial.println(data.size());
                Serial.print("process Action Data Content ");
                Serial.println(data.size()); */
                if (data.size() <= 0)
                    return;

                for (size_t i = 0; i < _serverCommands.size(); i++)
                {
                    /* Serial.print("searching Action ");
                    Serial.println(_selectedAddress);
                    Serial.print("Action Address ");
                    Serial.println(_serverCommands[i]->getAddress()); */
                    if (_serverCommands[i]->getAddress() == _selectedAddress)
                    {
                        _serverCommands[i]->makeCommand(&data);
                    }
                }
            }

        public:
            static ServerI2C *instance;
            ServerI2C(uint8_t address, int amountDataBytes) : DeviceI2C(), AMOUNT_DATA_BYTES(amountDataBytes)
            {
                instance = this;
                _deviceAddress = address;
                //_amountDataBytes = amountDataBytes;
                const int MAX = 100;
                CommandHandlerI2C *arrayC[MAX];
                _serverCommands.setStorage(arrayC);
                QueryHandlerI2C *arrayQ[MAX];
                _serverQueries.setStorage(arrayQ);
            };
            static ServerI2C *create(uint8_t address, uint8_t amountDataBytes)
            {
                if (instance == nullptr)
                {
                    ServerI2C newInstance(address, amountDataBytes);
                    instance = &newInstance;
                }
                return instance;
            }
            TwoWire *getI2C()
            {
                return i2c;
            }
            void processQuery()
            {
                ResponseData response;
                for (size_t i = 0; i < _serverQueries.size(); i++)
                {
                    if (_serverQueries[i]->getAddress() == _selectedAddress)
                    {
                        response = _serverQueries[i]->makeQueryResponse();
                        break;
                    }
                }
                if (response.size() == 0)
                {
                    const int MAX = 10;
                    byte array[MAX];
                    ResponseData data(array);
                    response = data;
                }
                u8 ok = 1;
                for (size_t i = 0; i < 2; i++)
                {
                    if (i < response.size())
                    {
                        ok = i2c->write(response.operator[](i));
                    }
                    else
                    {
                        ok = i2c->write(byte(0x0));
                    };
                    if(!ok) {
                        i2c->clearWriteError();
                    }
                }
            };
            void receivedCommand(int length)
            {
                byte address = i2c->read();
                this->setAddress(address);
                const int MAX = 10;
                byte array[MAX];
                CommandData data(array);
                int test = i2c->available();
                while (test)
                {
                    byte dato = i2c->read();
                    if (dato != -1)
                    {
                        data.push_back(dato);
                    }
                    test = i2c->available();
                }
                this->processCommand(data);
            };
            void addCommandHandler(CommandHandlerI2C &command)
            {
                _serverCommands.push_back(&command);
            }
            void addQueryHandler(QueryHandlerI2C &query)
            {
                _serverQueries.push_back(&query);
            }
            void begin()
            {
                i2c->begin(_deviceAddress);
                i2c->setTimeout(5000);
            };
            ~ServerI2C(){};
        };

        ServerI2C *ServerI2C::instance;

    } // namespace com_i2c
} // namespace acuaris

#endif