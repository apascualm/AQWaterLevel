#ifndef ACUARIS_COMMAND_HANDLER_I2C_H
#define ACUARIS_COMMAND_HANDLER_I2C_H

#include <Arduino.h>
#include <Vector.h>
namespace acuaris
{
    namespace com_i2c
    {
        class CommandHandlerI2C;
        typedef Vector<CommandHandlerI2C *> Commands;
        typedef Vector<byte> CommandData;

        class CommandHandlerI2C
        {
        protected:
            byte _address;

        public:
            CommandHandlerI2C(byte address)
            {
                _address = address;
            };
            ~CommandHandlerI2C(){};
            static void (*user_callback)(CommandData *data);
            byte getAddress()
            {
                return _address;
            };
            void setCallBack(void (*callback)(CommandData *data))
            {
                user_callback = callback;
            }
            void makeCommand(CommandData *data)
            {
                if (!user_callback)
                    return;
                user_callback(data);
            };
        };
        void (*CommandHandlerI2C::user_callback)(CommandData *);

    }
}

#endif