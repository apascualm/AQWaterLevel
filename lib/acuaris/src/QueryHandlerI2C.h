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
        int failcheckCallback = 0;

        class QueryHandlerI2C
        {
        protected:
            byte _address;
            ResponseData (*user_callback)();
        public:
            QueryHandlerI2C(byte address)
            {
                _address = address;
            }
            QueryHandlerI2C(byte address, ResponseData (*callback)())
            {
                _address = address;
                user_callback = callback;
            }
            ~QueryHandlerI2C(){};
            
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
                if (!user_callback)
                {
                    failcheckCallback++;
                    ResponseData data;
                    return data;
                }
                return user_callback();
            };
        };

        ///ResponseData (*QueryHandlerI2C::user_callback)();
    }
}

#endif