#include <iostream>
#include <hiredis/hiredis.h>
#include <unistd.h>
#include "user.hpp"
#include "myred.hpp"

enum choice{
    INDIVIDUAL,
    GROUP
};

class UserList{
private:
    redisContext *context;
    uint32_t gid;
    uint32_t owner;
    uint32_t admin;
public:
    UserList(redisContext *pcontext,int chce,uint32_t groupid):
        context(pcontext),gid(groupid){
            if (chce == INDIVIDUAL){
                owner = gid;
            }
        }
    bool badReply(redisReply* reply,std::string value){
        if (reply == NULL){
            std::cout << "bad reply when getlist" << std::endl;
            freeReplyObject(reply);
            return 1;
        }
        return 0;
    }

    void addMember(uint32_t uid);
    void delMember(uint32_t uid);
    std::vector<uint32_t> get_list();
    bool isMember(uint32_t uid);
};

void UserList::addMember(uint32_t uid)
{
    redisReply* reply = (redisReply*)redisCommand(context,"SADD %u:userlist %u",gid,uid);
    badReply(reply,"addMember");

    freeReplyObject(reply);
    return;
}
void UserList::delMember(uint32_t uid)
{
    redisReply* reply = (redisReply*)redisCommand(context,"SREM %u:userlist %u",gid,uid);
    badReply(reply,"addMember");

    freeReplyObject(reply);
    return;
}

std::vector<uint32_t> UserList::get_list()
{
    std::vector<uint32_t> retlist;
    redisReply* reply = (redisReply*)redisCommand(context,"SMEMBER %u:userlist",gid);
    if (badReply(reply,"get_list")){
        return {};
    }

    for (size_t i = 0; i < reply->elements ; i++)
    {
        uint32_t t = std::stoul(reply->element[i]->str);
        retlist.push_back(t);
    }

    freeReplyObject(reply);
    return retlist;
}

bool UserList::isMember(uint32_t uid)
{
    redisReply* reply = (redisReply*)redisCommand(context,"SISMEMBER %u:userlist %u",gid,uid);
    if (badReply(reply,"isMember")){
        return 0;
    }

    if (reply->integer == 1)
        return 1;
    return 0;
}
