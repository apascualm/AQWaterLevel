#ifndef ACUARIS_CLIENT_I2C_H
#define ACUARIS_CLIENT_I2C_H

#include <DeviceI2C.h>
#include <Command.h>
#include <Query.h>

namespace acuaris
{
    namespace com_i2c
    {
        class ClientI2C : public DeviceI2C
        {
        protected:
        public:
            ClientI2C(/* args */) : DeviceI2C()
            {
            }
            void begin()
            {
                i2c->begin();
            }
            void executeCommand(Command *command)
            {
                i2c->beginTransmission(*command->getDeviceAddress());
                i2c->write(*command->getMemoryAddress());
                CommandData *data = command->getData();
                if (data->size() > 0)
                {
                    for (size_t i = 0; i < data->size(); i++)
                    {
                        i2c->write(data->operator[](i));
                    }
                }
                i2c->endTransmission();
            }
            void executeQuery(Query *query) {

                i2c->beginTransmission(*query->getDeviceAddress());
                i2c->write(*query->getMemoryAddress());
                i2c->endTransmission();
                delay(200);

                i2c->requestFrom(*query->getDeviceAddress(), *query->getAoumtData());
                QueryData data = emptyQueryData();
                while (i2c->available())
                {
                    data.push_back(i2c->read());
                }
                query->executeCallback(&data);
            }
            CommandData emptyCommandData()
            {
                const int MAX = 10;
                byte array[MAX];
                CommandData data(array);
                return data;
            }
            QueryData emptyQueryData()
            {
                const int MAX = 10;
                byte array[MAX];
                QueryData data(array);
                return data;
            }
            ~ClientI2C() {}
        };
    } // namespace com_i2c

} // namespace acuaris

#endif