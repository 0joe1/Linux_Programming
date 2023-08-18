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
    redisReply* lpush(std::string key,std::string value)
    {
        redisReply* reply;
        reply = (redisReply*)redisCommand(context,"LPUSH %s %s",key.c_str(),value.c_str());
        return reply;
    }
    std::string rpop(std::string key)
    {
        redisReply* reply;
        std::string ret;
        reply = (redisReply*)redisCommand(context,"RPOP %s",key.c_str());

        this->badReply(reply);

        if (reply->type == REDIS_REPLY_NIL){
            freeReplyObject(reply);
            return "";
        }
        ret = reply->str;
        freeReplyObject(reply);
        return ret;
    }
    bool hasValue(std::string key,std::string value)
    {
        redisReply* reply;
        reply = (redisReply*)redisCommand(context,"LRANGE %s 0 -1",key.c_str());
        this->badReply(reply);

        for (size_t i=0;i<reply->elements;i++)
        {
            if (reply->element[i]->str == value){
                return 1;
            }
        }
        return 0;
    }

    std::string lpop(std::string key)
    {
        redisReply* reply;
        std::string ret;
        reply = (redisReply*)redisCommand(context,"LPOP %s",key.c_str());

        this->badReply(reply);

        if (reply->type == REDIS_REPLY_NIL){
            freeReplyObject(reply);
            return "";
        }
        ret = reply->str;
        freeReplyObject(reply);
        return ret;
    }
    bool isnull(std::string key)
    {
        redisReply* reply;
        reply = (redisReply*)redisCommand(context,"LLEN %s",key.c_str());

        this->badReply(reply);

        if (reply->integer == 0){
            freeReplyObject(reply);
            return 1;
        }
        freeReplyObject(reply);
        return 0;
    }
    std::vector<std::string> getkeys(std::string key)
    {
        redisReply* reply;
        std::cout << "KEYS *" << key << std::endl;
        reply = (redisReply*)redisCommand(context,"KEYS *%s",key.c_str());
        std::vector<std::string> retlist;
        for (size_t i = 0; i < reply->elements ; i++)
        {
            std::string t = reply->element[i]->str;
            retlist.push_back(t);
        }

        freeReplyObject(reply);
        return retlist;
    }
    int getlen(std::string key)
    {
        redisReply* reply;
        reply = (redisReply*)redisCommand(context,"LLEN %s",key.c_str());

        this->badReply(reply);

        return reply->integer;
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
