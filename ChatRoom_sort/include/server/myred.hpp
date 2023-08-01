#ifndef MYRED
#define MYRED
#include <iostream>
#include <hiredis/hiredis.h>
#include <string>
#include "user.hpp"


bool isOnline(redisReply *reply)
{
    std::string info = reply->str;
    User user(info);
    return user.status;
}
class Hred{
friend bool isOnline(redisReply *reply);
private:
    redisContext* context;
public:
    Hred(redisContext* pcontext):
        context(pcontext){}
    redisReply* set(uint32_t key,std::string value)
    {
        redisReply *reply;
        reply = (redisReply*)redisCommand(context,"SET %ld %b",key,value.data(),value.length());
        this->badReply(reply);
        return reply;
    }
    redisReply* get(uint32_t key)
    {
        redisReply *reply;
        reply = (redisReply*)redisCommand(context,"GET %ld",key);
        this->badReply(reply);
        return reply;
    }
    redisReply* del(uint32_t key)
    {
        redisReply *reply;
        reply = (redisReply*)redisCommand(context,"DEL %ld %s",key);
        return reply;
    }
    bool badReply(redisReply *);
};


bool Hred::badReply(redisReply *reply)
{
    if (reply == NULL){
        std::cout << "Bad reply" <<std::endl;
        freeReplyObject(reply);
        return 1;
    }
    return 0;
}


#endif
