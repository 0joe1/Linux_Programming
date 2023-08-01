#ifndef COMMAND
#define COMMAND
#include <iostream>
#include <stdint.h>
#include "srMsg.h"
#include "myred.hpp"
#include "user.hpp"
#include "menu.hpp"
#include "thread_pool.hpp"

extern std::map<uint32_t,int> fdMap;

enum tasks {
    LOGIN,
    SIGNUP,
    FRIENDCHAT
};

bool isNumeric(std::string const &str)
{
    auto it = str.begin();
    while (it != str.end() && std::isdigit(*it)) {
        it++;
    }
    return !str.empty() && it == str.end();
}

uint32_t readUint(int fd){
    std::string t;
    t = readMsg(fd);
    if (!isNumeric(t))
        printf("Please input a number\n");

    return std::stoul(t);
}

struct tasklist{
    static void menu(void*);
    static void login(void*);
    static void signup(void*);
    static void friendChat(void*);
};

class Command{
friend tasklist;

private:
    redisContext *context;
    thread_pool * pool;
    int fd;
    uint32_t uid;
    bool status=0;
public:
    Command(int cmfd,redisContext *cmcontext,thread_pool* cmpool):
        fd(cmfd),context(cmcontext),pool(cmpool){}
    /*
    thread_pool *getPool(){
        return this->pool;
    }
    redisContext *getContext(){
        return this->context;
    }
    bool getStatus(){
        return this->status;
    }*/
    unique_ptr<TASK> parse_command();
};


//void tasklist::menu(void* arg)
//{
    //Command *command = static_cast<Command*>(arg);
    //thread_pool *pool = command->pool;
//
    //readMsg(command->fd);
    //std::unique_ptr<TASK> task = command->parse_command();
    //pool->add_task(std::move(task));
//}

void tasklist::login(void* arg)
{
    bool status;
    Command *cmd = static_cast<Command*>(arg);
    thread_pool *pool = cmd->pool;
    Hred hred(cmd->context);
    std::string password,name;

    sendMsg(cmd->fd,"Please input UID");
    uint32_t uid;
    uid = readUint(cmd->fd);
    fdMap[uid] = cmd->fd;

    redisReply *reply = hred.get(uid);
    if (hred.badReply(reply)){
        return ;
    }
    if (reply->type == REDIS_REPLY_NIL){
        sendMsg(cmd->fd,"Please sign up first");
        return ;
    }

    nlohmann::json j = nlohmann::json::parse(reply->str);
    freeReplyObject(reply);
    User user(j);

    int limit=3;
    while (limit){
        sendMsg(cmd->fd,"Please input password");
        password = readMsg(cmd->fd);
        if (password == user.password){
            sendMsg(cmd->fd,"登陆成功");
            break;
        }
        limit--;
        std::string send = "wrong password, " + std::to_string(limit)+"chances remain";
       sendMsg(cmd->fd,send.c_str());
    }
    if (limit == 0)
        return;

    user.status = 1;
    hred.set(uid,user.getStr());

    cmd->status = 1;
    pool->add_task(std::move(cmd->parse_command()));
}
void tasklist::signup(void* arg)
{
    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);
    uint32_t uid;
    std::string name,password;

    sendMsg(cmd->fd,"Please input UID");
    uid = readUint(cmd->fd);
    redisReply *reply = hred.get(uid);
    if (hred.badReply(reply)){
        freeReplyObject(reply);
        return ;
    }
    if (reply->type != REDIS_REPLY_NIL){
        sendMsg(cmd->fd,"The user already exists");
        freeReplyObject(reply);
        return ;
    }
    freeReplyObject(reply);

    sendMsg(cmd->fd,"Please input your password");
    password = readMsg(cmd->fd);
    sendMsg(cmd->fd,"Please input your nickname");
    name = readMsg(cmd->fd);

    User user(uid,password,name);
    user.status = 1;
    redisReply *r= hred.set(uid,user.getStr());
    if (hred.badReply(r)){
        freeReplyObject(r);
        return ;
    }
    if (strcmp(r->str,"OK")==0)
        sendMsg(cmd->fd,"注册成功");
    else
        sendMsg(cmd->fd,"注册出问题了...");

    freeReplyObject(r);
    return ;
}

void tasklist::friendChat(void* arg)
{
    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);
    thread_pool *pool = cmd->pool;
    if (cmd->status == 0){
        pool->add_task(cmd->parse_command());
        return;
    }

    uint32_t fuid = readUint(cmd->fd);
    redisReply *reply = hred.get(fuid);
    while (reply->type == REDIS_REPLY_NIL){
        sendMsg(cmd->fd,"未找到该用户，请重试");
        reply = hred.get(fuid);
        return ;
    }
    while (!isOnline(reply)){
        sendMsg(cmd->fd,"该用户未上线");
        return ;
    }

    int ffd = fdMap[fuid];
    std::string msg;
    while(1)
    {
        msg = readMsg(cmd->fd);
        sendMsg(ffd,msg.c_str());
    }

}

void test(void *arg)
{
    std::cout << "test succeed" << std::endl;
}

unique_ptr<TASK> Command::parse_command()
{
    tasklist funcs;
    uint32_t choice = readUint(fd);
    std::unique_ptr<TASK> work = std::make_unique<TASK>();

    work->arg = this;
    switch(choice)
    {
        case 0:
            std::cout << "login" << std::endl;
            work->func = funcs.login;
            return work;
        case 1:
            std::cout << "signup" << std::endl;
            work->func = funcs.signup;
            break;
        case 2:
            work->func = funcs.friendChat;
        default:;
    }

    return work;
}


#endif
