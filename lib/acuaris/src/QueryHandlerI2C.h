#ifndef ACUARIS_QUERY_HANDLER_I2C_H
#define ACUARIS_QUERY_HANDLER_I2C_H

#include <Arduino.h>
#include <Vector.h>

namespace acuaris
{
    namespace com_i2c
    {
        class QueryHandlerI2C;
        typedef Vector<QueryHandlerI2C *> Queries;
        typedef Vector<byte> ResponseData;

        class QueryHandlerI2C
        {
        protected:
            byte _address;
        public:
            QueryHandlerI2C(byte address)
            {
                _address = address;
            }
            QueryHandlerI2C(byte address, ResponseData (*callback)())
            {
                _address = address;
            }
            ~QueryHandlerI2C(){};
            static ResponseData (*user_callback)();
            byte getAddress()
            {
                return _address;
            };
            void setCallBack(ResponseData (*callback)())
            {
                user_callback = callback;
            }
            ResponseData makeQueryResponse()
            {
                Serial.println("response data");
                if (user_callback == nullptr)
                {
                    Serial.println("response empty data");
                    ResponseData data;
                    return data;
                }
                return this->user_callback();
            };
        };

        ResponseData (*QueryHandlerI2C::user_callback)();
    }
}

#endif