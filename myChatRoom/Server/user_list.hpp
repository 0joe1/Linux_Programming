#include <iostream>
#include <hiredis/hiredis.h>
#include <unistd.h>
#include "../Common/myred.hpp"
#include "../Common/srMsg.hpp"
#include "history.hpp"

enum choice{
    INDIVIDUAL,
    GROUP
};


class UserList{
private:
    redisContext *context;
    const char* listType;
    uint32_t gid;
    uint32_t owner{0};
    uint32_t admin;
public:
    UserList(redisContext *pcontext,const char* type,int chce,uint32_t groupid):
        context(pcontext),listType(type),gid(groupid){
            if (chce == INDIVIDUAL){
                owner = gid;
            }
            else {
                redisReply* reply = (redisReply*)redisCommand(context,"HGET %u:owner owner",gid);
                if (reply->type == REDIS_REPLY_NIL)
                    return;
                owner = std::stoul(reply->str);
            }
        }
    void setOwner(uint32_t newOwner){
        owner = newOwner;
        redisCommand(context,"HSET %u:owner owner %u",gid,newOwner);
    }
    uint32_t getOwner(void){
        return owner;
    }
    bool badReply(redisReply* reply,std::string value){
        if (reply == NULL){
            std::cout << "bad reply when" << value << std::endl;
            freeReplyObject(reply);
            return 1;
        }
        return 0;
    }

    void delGroup();
    bool hasGroup();

    void addMember(uint32_t uid);
    void delMember(uint32_t uid);
    void addAdmin(uint32_t uid);
    void delAdmin(uint32_t uid);

    std::vector<uint32_t> get_list();
    std::vector<uint32_t> get_admin_list();

    bool isMember(uint32_t uid);
    bool isAdmin(uint32_t uid);
    bool isOwner(uint32_t uid);

    bool groupOnline(std::map<uint32_t,int> fdMap);

    void send_to_high(std::map<uint32_t,int>,rMsg*,std::string);
    void send_to_all(std::map<uint32_t, int> fdMap,rMsg* smsg,std::string content);
};

void UserList::addMember(uint32_t uid)
{
    redisReply* reply = (redisReply*)redisCommand(context,"SADD %u:%s %u",gid,listType,uid);
    badReply(reply,"addMember");

    freeReplyObject(reply);
    return;
}
void UserList::delMember(uint32_t uid)
{
    redisReply* reply = (redisReply*)redisCommand(context,"SREM %u:%s %u",gid,listType,uid);
    badReply(reply,"delMember");

    freeReplyObject(reply);
    return;
}
void UserList::addAdmin(uint32_t uid)
{
    if (!isMember(uid)){
        addMember(uid);
    }
    redisReply* reply = (redisReply*)redisCommand(context,"SADD %u:admin %u",gid,uid);
    badReply(reply,"addAdmin");

    freeReplyObject(reply);
    return;
}
void UserList::delAdmin(uint32_t uid)
{
    redisReply* reply = (redisReply*)redisCommand(context,"SREM %u:admin %u",gid,uid);
    badReply(reply,"delAdmin");

    freeReplyObject(reply);
    return;
}

std::vector<uint32_t> UserList::get_list()
{
    std::vector<uint32_t> retlist;
    redisReply* reply = (redisReply*)redisCommand(context,"SMEMBERS %u:%s",gid,listType);
    if (badReply(reply,"get_list")){
        return {};
    }
    printf("%zu\n",reply->elements);

    for (size_t i = 0; i < reply->elements ; i++)
    {
        uint32_t t = std::stoul(reply->element[i]->str);
        retlist.push_back(t);
    }

    freeReplyObject(reply);
    return retlist;
}

std::vector<uint32_t> UserList::get_admin_list()
{
    std::vector<uint32_t> retlist;
    redisReply* reply = (redisReply*)redisCommand(context,"SMEMBERS %u:admin",gid);
    if (badReply(reply,"get_list")){
        return {};
    }
    printf("admin_list:%zu\n",reply->elements);

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
    redisReply* reply = (redisReply*)redisCommand(context,"SISMEMBER %u:%s %u",gid,listType,uid);
    if (badReply(reply,"isMember")){
        return 0;
    }

    if (reply->integer == 1){
        freeReplyObject(reply);
        return 1;
    }
    freeReplyObject(reply);
    return 0;
}

bool UserList::isAdmin(uint32_t uid)
{
    redisReply* reply = (redisReply*)redisCommand(context,"SISMEMBER %u:admin %u",gid,uid);
    if (badReply(reply,"isMember")){
        return 0;
    }

    if (reply->integer == 1){
        freeReplyObject(reply);
        return 1;
    }
    freeReplyObject(reply);
    return 0;
}

bool UserList::isOwner(uint32_t uid)
{
    return uid == owner;
}
bool UserList::hasGroup()
{
    redisReply* reply = (redisReply*)redisCommand(context,"HGET %u:owner owner",gid);
    badReply(reply,"hasGroup");
    if (reply->type == REDIS_REPLY_NIL){
        freeReplyObject(reply);
        return 0;
    }
    freeReplyObject(reply);

    return 1;
}

bool UserList::groupOnline(std::map<uint32_t,int> fdMap)
{
    if (fdMap[owner] > 0){
        return 1;
    }
    std::vector<uint32_t> admvec = get_admin_list();
    for (auto admin : admvec)
    {
        if (fdMap[admin] > 0)
            return 1;
    }
    return 0;
}

void UserList::send_to_high(std::map<uint32_t,int> fdMap,rMsg* smsg,std::string content)
{
    if (fdMap[owner] > 0){
        int ownfd = fdMap[owner];
        sendmg(ownfd,smsg,content);
    }
    std::vector<uint32_t> admvec = get_admin_list();
    for (auto admin:admvec)
    {
        if (fdMap[admin]>0)
        {
            int admfd = fdMap[admin];
            sendmg(admfd,smsg,content);
        }
    }
}

void UserList::send_to_all(std::map<uint32_t, int> fdMap,rMsg* smsg,std::string content)
{
    if (fdMap[owner] > 0){
        History history(context,owner,"groupchat",20);
        history.get_hismsg();
        int ownfd = fdMap[owner];
        sendmg(ownfd,smsg,content);
    }
    std::vector<uint32_t> usvt = get_list();
    for (auto member:usvt)
    {
        if (fdMap[member]>0)
        {
            History history(context,owner,"groupchat",20);
            history.get_hismsg();
            int memfd = fdMap[member];
            sendmg(memfd,smsg,content);
        }
    }

}

void UserList::delGroup()
{
    redisCommand(context,"DEL %u:userlist",gid);
    redisCommand(context,"DEL %u:admin",gid);
    redisCommand(context,"DEL %u:owner",gid);
}
