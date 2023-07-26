#ifndef COMMAND
#define COMMAND
#include <iostream>
#include <stdint.h>
#include "srMsg.h"
#include "myred.hpp"
#include "user.hpp"
#include "thread_pool.hpp"


enum tasks {
    LOGIN,
    SIGNUP
};

uint32_t readUint(int fd){
    std::string t;
    t = readMsg(fd);

    return std::stoul(t);
}

class Command{
private:
    redisContext *context;
    int fd;
    uint32_t uid;
public:
    Command(int cmfd,redisContext *cmcontext):
        fd(cmfd),context(cmcontext){}
    unique_ptr<TASK> parse_command();
};


bool login(int fd,redisContext *pcontext)
{
    std::cout << "Please input UID" << std::endl;
    Hred hred(pcontext);
    uint32_t uid;
    std::string password,name;
    uid = readUint(fd);

    redisReply *reply = hred.get(uid);
    if (hred.badReply(reply)){
        return 0;
    }
    if (reply->type == REDIS_REPLY_NIL){
        std::cout << "Please sign up first" <<std::endl;
        return 0;
    }

    nlohmann::json j = nlohmann::json::parse(reply->str);
    User user(j);

    int limit=3;
    while (limit){
        std::cout << "Please input password" << std::endl;
        password = readMsg(fd);
        if (password == user.password){
            std::cout << "登陆成功 ~" << std::endl;
            return 1;
        }
        limit--;
        std::cout << "wrong password, " << limit << "chances remain"<< std::endl;
    }

    return 0;
}
bool signup(int fd,redisContext *pcontext)
{
    Hred hred(pcontext);
    uint32_t uid;
    std::string name,password;

    std::cout << "Please input UID" << std::endl;
    uid = readUint(fd);
    redisReply *reply = hred.get(uid);
    if (hred.badReply(reply)){
        return 0;
    }
    if (reply->type != REDIS_REPLY_NIL){
        std::cout << "The user already exists" <<std::endl;
        return 0;
    }

    std::cout << "Please input your password" << std::endl;
    password = readMsg(fd);
    std::cout << "Please input your nickname" << std::endl;
    name = readMsg(fd);

    User user(uid,password,name);
    nlohmann::json j = user.tojson();
    hred.set(uid,j.dump());

    return 1;
}

void test(void *arg)
{
    std::cout << "test succeed" << std::endl;
}

unique_ptr<TASK> Command::parse_command()
{
    uint32_t task = readUint(fd);
    std::unique_ptr<TASK> work = std::make_unique<TASK>();

    switch(task)
    {
        case 0:
            std::cout << "login" << std::endl;
            break;
        case 1:
            std::cout << "signup" << std::endl;
            break;
        default:;
    }
    work->func = test;

    return work;
}









#endif
