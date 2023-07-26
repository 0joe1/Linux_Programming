#include <iostream>
#include <hiredis/hiredis.h>
#include <unistd.h>
#include "user.hpp"
#include "myred.hpp"


class UserList{
private:
    Hred hred;
    uint32_t owner;
    uint64_t admin;
public:
    UserList(redisContext *pcontext):
        hred(pcontext){}
};

int main()
{
    redisContext *c = redisConnect("127.0.0.1",6379);
    if (c==NULL || c->err)
    {
        if (c)
            printf("Error:%s\n",c->errstr);
        else
            printf("Can't allocate redis context\n");
    }
    UserList test(c);

    //struct user a,b,e;
    //a.uid=1;a.name="zhangsan";a.password="12345";
    //b.uid=2;b.name="lisi";b.password="4399";
    //e.uid=666;e.name="chusheng";e.password="38666";
//
    //nlohmann::json j;
    //j["uid"]=e.uid;
    //j["name"]=e.name;
    //j["password"]=e.password;
//
    //test.set(e.uid,j.dump());
    //std::cout << "Serialized data: " << j.dump() << std::endl;
//
    //redisReply *reply = test.get(e.uid);
    //nlohmann::json parsed_data = nlohmann::json::parse(reply->str);
    //std::cout << parsed_data["password"] <<std::endl;

    freeReplyObject(reply);
    redisFree(c);
    return 0;
}
//g++ -o user_list user_list.cpp -lhiredis
