#ifndef ROI_SERVER_H
#define ROI_SERVER_H

#include <ServerI2C.h>

namespace acuaris
{
    using namespace com_i2c;
    class AWLServer : public ServerI2C
    {
    private:
        /* data */
    public:
        AWLServer() : ServerI2C(100, 2)
        {
        }
        ~AWLServer(){};
        ResponseData emptyResponseData()
        {
            const int MAX = 2;
            byte array[MAX];
            ResponseData data(array);
            return data;
        }
    };

    class GenericAWLCommandHandler : public CommandHandlerI2C
    {
    private:
    public:
        GenericAWLCommandHandler(byte address) : CommandHandlerI2C(address){};
        GenericAWLCommandHandler(byte address, void (*callback)(CommandData *data)) : CommandHandlerI2C(address)
        {
            user_callback = callback;
        };
        ~GenericAWLCommandHandler(){};
    };

    class GenericAWLQueryHandler : public QueryHandlerI2C
    {
    private:
    public:
        GenericAWLQueryHandler(byte address) : QueryHandlerI2C(address){};
        GenericAWLQueryHandler(byte address, ResponseData (*callback)()) : QueryHandlerI2C(address, callback)
        {
        
        }
        ~GenericAWLQueryHandler(){}
    };

}

#endif