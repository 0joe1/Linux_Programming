#ifndef MYRED
#define MYRED
#include <iostream>
#include <hiredis/hiredis.h>
#include <string>

class Hred{
private:
    redisContext* context;
public:
    Hred(redisContext* pcontext):
        context(pcontext){}
    redisReply* set(uint32_t key,std::string value)
    {
        redisReply *reply;
        reply = (redisReply*)redisCommand(context,"SET %ld %b",key,value.data(),value.length());
        return reply;
    }
    redisReply* get(uint32_t key)
    {
        redisReply *reply;
        reply = (redisReply*)redisCommand(context,"GET %ld",key);
        return reply;
    }
    redisReply* del(uint32_t key)
    {
        redisReply *reply;
        reply = (redisReply*)redisCommand(context,"DEL %ld %s",key);
        return reply;
    }
    bool badReply(redisReply *pcontext);
};


bool Hred::badReply(redisReply *reply)
{
    if (reply == NULL){
        std::cout << "Failed to execute GET command" <<std::endl;
        return 1;
    }

    return 0;
}

#endif
