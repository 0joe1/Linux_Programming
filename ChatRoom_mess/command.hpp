#ifndef COMMAND
#define COMMAND
#include <iostream>
#include <stdint.h>
#include <sys/epoll.h>
#include "srMsg.hpp"
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
    int epfd;
    int fd;
    uint32_t uid;
    std::string m;
public:
    Command(int cmfd,redisContext *cmcontext,thread_pool* cmpool,int cmepfd):
        fd(cmfd),context(cmcontext),pool(cmpool),epfd(cmepfd){}
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


void epoll_add(int fd,int epfd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
}


void tasklist::login(void* arg)
{
    char *str;
    rMsg smsg; //client receive what server send
    smsg.flag = 0;

    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);
    std::string password;
    uint32_t uid;


    printf("test1\n");
    Msg msg(cmd->m);
    printf("test2\n");
    uid = msg.uid;


    redisReply *reply = hred.get(uid);
    if (hred.badReply(reply)){
        return ;
    }
    if (reply->type == REDIS_REPLY_NIL){
        smsg.mg = "请先注册";
        sendMsg(cmd->fd,smsg.toStr().c_str());
        return ;
    }

    nlohmann::json j = nlohmann::json::parse(reply->str);
    freeReplyObject(reply);
    User user(j);

    password = msg.password;
    if (password != user.password){
        smsg.mg = "密码错误";
        sendMsg(cmd->fd,smsg.toStr().c_str());
        return;
    }
    //防止重复登陆
    if (fdMap.count(uid) > 0){
        smsg.mg = "已登陆";
        sendMsg(cmd->fd,smsg.toStr().c_str());
        return;
    }
    fdMap[uid] = cmd->fd;
    smsg.mg = "登陆成功";
    std::cout << smsg.mg << std::endl;
    sendMsg(cmd->fd,smsg.toStr().c_str());

    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::signup(void* arg)
{
    char *str;
    rMsg smsg;
    smsg.flag = 1;

    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);
    uint32_t uid;
    std::string name,password;

    Msg msg(cmd->m);

    uid = msg.uid;
    redisReply *reply = hred.get(uid);
    if (hred.badReply(reply)){
        freeReplyObject(reply);
        return ;
    }
    if (reply->type != REDIS_REPLY_NIL){
        smsg.mg = "该用户已存在";
        sendMsg(cmd->fd,smsg.toStr().c_str());
        freeReplyObject(reply);
        return ;
    }
    freeReplyObject(reply);

    password = msg.password;

    User user(uid,password);
    redisReply *r= hred.set(uid,user.getStr());
    if (hred.badReply(r)){
        freeReplyObject(r);
        return ;
    }
    std::string t;
    if (strcmp(r->str,"OK")==0)
        t = "注册成功";
    else
        t = "注册出问题";
    //rMsg2Str(&smsg,t.c_str(),&str);
    //std::cout << smsg.mg << std::endl;
    smsg.mg = t;
    sendMsg(cmd->fd,smsg.toStr().c_str());


    freeReplyObject(r);
    epoll_add(cmd->fd,cmd->epfd);
    return ;
}

void sendmg(int fd,rMsg *msg,std::string mg)
{
    msg->mg = mg;
    sendMsg(fd,msg->toStr().c_str());
}

void tasklist::friendChat(void* arg)
{
    rMsg smsg;
    smsg.flag = 2;
    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);

    Msg msg(cmd->m);

    int fuid,touid;
    fuid  = msg.uid;
    touid = msg.touid;
    redisReply *reply = hred.get(touid);
    while (reply->type == REDIS_REPLY_NIL){
        sendmg(cmd->fd,&smsg,"未找到该用户，请重试");
        return ;
    }

    std::string content  =  msg.content;
    hred.lpush(fuid,touid,content);

    int tofd;
    if (fdMap.count(touid) == 0){
        sendmg(cmd->fd,&smsg,"该用户未上线");
        return ;
    }
    tofd = fdMap[touid];
    while ((content=hred.rpop(fuid,touid)) != "" ){
        sendmg(tofd,&smsg,content);
    }
    epoll_add(cmd->fd,cmd->epfd);
}

void test(void *arg)
{
    std::cout << "test succeed" << std::endl;
}

unique_ptr<TASK> Command::parse_command()
{
    tasklist funcs;
    std::unique_ptr<TASK> work = std::make_unique<TASK>();

    this->m = readMsg(fd);
    Msg msg(this->m);

    int choice = msg.flag;
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
            break;
        default:;
    }
    std::cout << m << std::endl;

    return work;
}


#endif
