#include <iostream>
#include <hiredis/hiredis.h>
#include "user.hpp"

class UserList{
private:
    redisContext* context;
public:
    void load_context(redisContext* pcontext){
        context = pcontext;
    }
    redisReply* set(uint32_t key,std::string value)
    {
        redisReply *reply;
        reply = (redisReply*)redisCommand(context,"SET %ld %s",key,value.c_str());
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
};

int main()
{
    UserList test;
    redisContext *c = redisConnect("127.0.0.1",6379);
    if (c==NULL || c->err)
    {
        if (c)
            printf("Error:%s\n",c->errstr);
        else
            printf("Can't allocate redis context\n");
    }
    test.load_context(c);

    struct user a,b,e;
    a.uid=1;a.name="zhangsan";a.password="12345";
    b.uid=2;b.name="lisi";b.password="4399";
    e.uid=666;e.name="chusheng";e.password="38666";
    test.set(a.uid,a.password);
    test.set(b.uid,b.password);
    test.set(e.uid,e.password);
    test.del(e.uid);
    redisReply *reply = test.get(e.uid);
    std::cout << reply->str <<std::endl;

    freeReplyObject(reply);
    redisFree(c);
    return 0;
}
