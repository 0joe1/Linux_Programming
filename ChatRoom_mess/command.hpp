#ifndef COMMAND
#define COMMAND
#include <iostream>
#include <stdint.h>
#include <sys/epoll.h>
#include "srMsg.hpp"
#include "myred.hpp"
#include "user.hpp"
#include "thread_pool.hpp"
#include "user_list.hpp"

extern std::map<uint32_t,int> fdMap;

enum tasks {
    LOGIN,
    SIGNUP,
    FRIENDCHAT,
    SHOWFRIEND,
    ADDFRIEND,
    DELFRIEND,
    FRIENDREQUEST,
    CREATGROUP,
    GROUPCHAT,
    SHOWGROUP,
    ADDMEMBER,
    KICKMEMBER,
    ADDADMIN,
    DELADMIN,
    ADDGROUP,
    GROUPREQUEST,
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
    static void menu        (void*);
    static void login       (void*);
    static void signup      (void*);
    static void friendChat  (void*);
    static void showFriend  (void*);
    static void addFriend   (void*);
    static void friend_req  (void*);
    static void delFriend   (void*);
    static void creatGroup  (void*);
    static void addMember   (void*);
    static void kickMember  (void*);
    static void addGroup    (void*);
    static void group_req   (void*);
    static void showGroup   (void*);
    static void groupChat   (void*);
    static void addAdmin    (void*);
    static void delAdmin    (void*);
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

redisReply* check_uexist(Hred hred,uint32_t uid,int fd,rMsg smsg,std::string value)
{
    redisReply *reply = hred.get(uid);
    if (hred.badReply(reply)){
        return NULL;
    }
    if (reply->type == REDIS_REPLY_NIL){
        smsg.mg = value;
        sendMsg(fd,smsg.toStr().c_str());
        return NULL;
    }

    return reply;
}

void epoll_add(int fd,int epfd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
}


void tasklist::login(void* arg)
{
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

    redisReply* reply;
    if((reply =check_uexist(hred,uid,cmd->fd,smsg,"请先注册")) == NULL){
        epoll_add(cmd->fd,cmd->epfd);
        return;
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
        epoll_add(cmd->fd,cmd->epfd);
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



void tasklist::friendChat(void* arg)
{
    rMsg smsg;
    smsg.flag = FRIENDCHAT;
    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);

    Msg msg(cmd->m);

    int fuid,touid;
    fuid  = msg.uid;
    touid = msg.touid;
    redisReply *reply = hred.get(touid);
    while (reply->type == REDIS_REPLY_NIL){
        sendmg(cmd->fd,&smsg,"未找到该用户，请重试");
        epoll_add(cmd->fd,cmd->epfd);
        return ;
    }

    chatMsg chatmsg;
    chatmsg.fid = fuid;
    chatmsg.content = msg.content;
    std::string key = std::to_string(fuid) + ":"+ std::to_string(touid) + ":privatechat";
    hred.lpush(key,chatmsg.toStr().c_str());

    std::string content;
    int tofd;
    if (fdMap.count(touid) == 0){
        sendmg(cmd->fd,&smsg,"该用户未上线");
        epoll_add(cmd->fd,cmd->epfd);
        return ;
    }
    tofd = fdMap[touid];
    while ((content=hred.rpop(key)) != "" ){
        sendmg(tofd,&smsg,content);
    }
    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::groupChat(void* arg)
{
    rMsg smsg;
    smsg.flag = GROUPCHAT;

    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);

    Msg msg(cmd->m);
    UserList grlt(cmd->context,"userlist",GROUP,msg.touid);
    if (!grlt.hasGroup()){
        sendmg(cmd->fd,&smsg,"查无此群!");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }

    chatMsg chatmsg;
    chatmsg.fid = msg.uid;
    chatmsg.gid = msg.touid;
    chatmsg.content = msg.content;
    std::string key = std::to_string(msg.uid) + ":"+ std::to_string(msg.touid) + ":groupchat";
    hred.lpush(key,chatmsg.toStr().c_str());

    grlt.send_to_all(fdMap,&smsg,chatmsg.toStr());

    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::showFriend(void* arg)
{
    rMsg smsg;
    smsg.flag = SHOWFRIEND;

    Command *cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);

    UserList uslt(cmd->context,"userlist",INDIVIDUAL,msg.uid);
    std::vector<uint32_t> usvc = uslt.get_list();

    std::stringstream ss;
    for (auto& frid : usvc)
    {
        printf("%u\n",frid);
        int online = 0;
        if (fdMap.count(frid) > 0)
            online = 1;
        ss << frid << " " << online << " ";
    }

    std::string result = ss.str();
    sendmg(cmd->fd,&smsg,result);

    epoll_add(cmd->fd,cmd->epfd);
}
void tasklist::showGroup(void* arg)
{
    rMsg smsg;
    smsg.flag = SHOWGROUP;

    Command* cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);

    UserList grlt(cmd->context,"userlist",GROUP,msg.uid);
    std::vector<uint32_t> users = grlt.get_list();

    std::stringstream ss;

    //owner 特殊处理下
    uint32_t owner = grlt.getOwner();
    int owneronline = 0;
    if (fdMap.count(owner) > 0)
        owneronline = 1;
    ss << owner << " " << 3 << " " << owneronline << " ";

    for (auto user : users)
    {
        int online = 0,level = 1;
        if (fdMap.count(user)>0)
            online = 1;
        if (grlt.isAdmin(user))
            level = 2;
        ss << user << " " << level << " " << online << " ";
    }
    std::string result = ss.str();
    sendmg(cmd->fd,&smsg,result);

    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::addFriend(void* arg)
{
    rMsg smsg;
    smsg.flag = ADDFRIEND;
    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);

    Msg msg(cmd->m);
    uint32_t fuid,touid;
    fuid  = msg.uid;
    touid = msg.touid;

    redisReply* reply;
    if((reply = check_uexist(hred,touid,cmd->fd,smsg,"未发现该用户")) == NULL){
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    freeReplyObject(reply);

    UserList uslt(cmd->context,"userlist",INDIVIDUAL,fuid);
    if (uslt.isMember(touid)){
        epoll_add(cmd->fd,cmd->epfd);
        sendmg(cmd->fd,&smsg,"TA已经是你的伙伴啦");
        return;
    }
    uslt.addMember(touid);

    int tofd;
    std::string key = std::to_string(touid) + ":friendrequest";
    std::string content;
    if ((content = hred.rpop(key)) == ""){
        content = std::to_string(fuid);
        hred.lpush(key,content);
    }
    if (fdMap.count(touid) == 0){
        epoll_add(cmd->fd,cmd->epfd);
        return ;
    }

    tofd = fdMap[touid];
    content = hred.rpop(key);

    sendmg(tofd,&smsg,content);

    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::friend_req(void* arg)
{
    rMsg smsg;
    smsg.flag = FRIENDREQUEST;

    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);
    uint32_t fuid,touid;

    Msg msg(cmd->m);
    fuid  =  msg.uid;
    touid =  msg.touid;

    int tofd = fdMap[touid];
    if (msg.content == "n"){
        std::string s = "您被用户" + std::to_string(fuid) + "无情地拒绝了";
        sendmg(tofd,&smsg,s);
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }

    UserList uslt(cmd->context,"userlist",INDIVIDUAL,fuid);
    uslt.addMember(touid);
    std::string s = "恭喜您，与用户" + std::to_string(fuid) + "双向奔赴";
    sendmg(tofd,&smsg,s);

    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::group_req(void* arg)
{
    rMsg smsg;
    smsg.flag = GROUPREQUEST;

    Command *cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);
    groupReq gq(msg.uid,msg.touid);

    int rfd = fdMap[msg.uid];
    UserList grlt(cmd->context,"userlist",GROUP,msg.touid);
    if (msg.content == "n")
    {
        gq.status = 2;
        std::string content = "您想要潜入群" + std::to_string(msg.touid) + "的幻想被" + std::to_string(msg.adduid)+"击碎了";
        sendmg(rfd,&smsg,content);
        grlt.send_to_high(fdMap,&smsg,gq.toStr());

        epoll_add(cmd->fd,cmd->epfd);
        return;
    }

    gq.status = 1;
    grlt.addMember(msg.uid);
    grlt.send_to_high(fdMap,&smsg,gq.toStr());
    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::delFriend(void* arg)
{
    rMsg smsg;
    smsg.flag = DELFRIEND;

    Command *cmd = static_cast<Command*>(arg);

    Msg msg(cmd->m);
    uint32_t touid = msg.touid;

    UserList uslt(cmd->context,"userlist",INDIVIDUAL,msg.uid);
    uslt.delMember(touid);
    sendmg(cmd->fd,&smsg,"删除成功哦！");

    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::creatGroup(void* arg)
{
    rMsg smsg;
    smsg.flag = CREATGROUP;

    Command *cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);
    Hred hred(cmd->context);

    UserList uslt(cmd->context,"userlist",GROUP,msg.touid);
    redisReply* reply = hred.get(msg.touid);

    if (reply->type != REDIS_REPLY_NIL || uslt.hasGroup()){
        sendmg(cmd->fd,&smsg,"该群id已被占用");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    uslt.setOwner(msg.uid);

    UserList grlt(cmd->context,"grouplist",INDIVIDUAL,msg.uid);
    grlt.addMember(msg.touid);


    sendmg(cmd->fd,&smsg,"创建群聊成功");
    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::addMember(void *arg)
{
    rMsg smsg;
    smsg.flag = ADDMEMBER;

    Command *cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);

    UserList grlt(cmd->context,"userlist",GROUP,msg.touid);
    if (!grlt.hasGroup()){
        sendmg(cmd->fd,&smsg,"查无此群！");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (!grlt.isAdmin(msg.uid) && !grlt.isOwner(msg.uid)){
        sendmg(cmd->fd,&smsg,"权限不足！");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (grlt.isMember(msg.adduid)){
        sendmg(cmd->fd,&smsg,"该用户已是群成员");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    grlt.addMember(msg.adduid);

    sendmg(cmd->fd,&smsg,"添加成员成功");
    epoll_add(cmd->fd,cmd->epfd);
}
void tasklist::kickMember(void* arg)
{
    rMsg smsg;
    smsg.flag = KICKMEMBER;

    Command *cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);

    //相应权限检查
    UserList grlt(cmd->context,"userlist",GROUP,msg.touid);
    if (!grlt.hasGroup()){
        sendmg(cmd->fd,&smsg,"查无此群！");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (grlt.isOwner(msg.adduid)){
        sendmg(cmd->fd,&smsg,"群主之位不能简单删除，只能转让");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (!grlt.isMember(msg.adduid)){
        sendmg(cmd->fd,&smsg,"你不能踢一个不存在的人");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (!grlt.isAdmin(msg.uid) && !grlt.isOwner(msg.uid)){
        sendmg(cmd->fd,&smsg,"您没有踢人的权限");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }

    if (grlt.isAdmin(msg.adduid))
    {
        if (!grlt.isOwner(msg.uid)){
            sendmg(cmd->fd,&smsg,"你不能踢一个比你等级高的人");
            epoll_add(cmd->fd,cmd->epfd);
            return;
        }
        grlt.delAdmin(msg.adduid);
        grlt.delMember(msg.adduid);
        sendmg(cmd->fd,&smsg,"踢人成功！");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    grlt.delMember(msg.adduid);
    sendmg(cmd->fd,&smsg,"踢人成功！");
    epoll_add(cmd->fd,cmd->epfd);
    return;
}

void tasklist::addAdmin(void* arg)
{
    rMsg smsg;
    smsg.flag = ADDADMIN;

    Command *cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);

    UserList grlt(cmd->context,"userlist",GROUP,msg.touid);
    if (!grlt.isOwner(msg.uid)){
        sendmg(cmd->fd,&smsg,"权限不足！");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (grlt.isAdmin(msg.adduid)){
        sendmg(cmd->fd,&smsg,"该用户已是管理员也");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    grlt.addAdmin(msg.adduid);

    sendmg(cmd->fd,&smsg,"添加管理员成功");
    epoll_add(cmd->fd,cmd->epfd);
}

void tasklist::delAdmin(void* arg)
{
    rMsg smsg;
    smsg.flag = DELADMIN;

    Command *cmd = static_cast<Command*>(arg);
    Msg msg(cmd->m);

    UserList grlt(cmd->context,"userlist",GROUP,msg.touid);
    if (!grlt.isOwner(msg.uid)){
        sendmg(cmd->fd,&smsg,"权限不足！");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (!grlt.isAdmin(msg.adduid)){
        sendmg(cmd->fd,&smsg,"删除成功");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    grlt.delAdmin(msg.adduid);

    sendmg(cmd->fd,&smsg,"删除管理员成功");
    epoll_add(cmd->fd,cmd->epfd);
}


void tasklist::addGroup(void *arg)
{
    rMsg smsg;
    smsg.flag = ADDGROUP;

    Command *cmd = static_cast<Command*>(arg);
    Hred hred(cmd->context);
    Msg msg(cmd->m);

    groupReq gq(msg.uid,msg.touid);
    UserList grlt(cmd->context,"userlist",GROUP,msg.touid);
    if (!grlt.hasGroup()){
        sendmg(cmd->fd,&smsg,"不存在该群");
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }
    if (!grlt.groupOnline(fdMap)){
        std::string key = std::to_string(msg.uid)+":"+std::to_string(msg.touid)+":grouprequest";
        if (hred.isnull(key))
            hred.lpush(key,gq.toStr().c_str());
        epoll_add(cmd->fd,cmd->epfd);
        return;
    }

    grlt.send_to_high(fdMap,&smsg,gq.toStr());
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
        case LOGIN:
            std::cout << "login" << std::endl;
            work->func = funcs.login;
            return work;
        case SIGNUP:
            std::cout << "signup" << std::endl;
            work->func = funcs.signup;
            break;
        case FRIENDCHAT:
            work->func = funcs.friendChat;
            break;
        case SHOWFRIEND:
            work->func = funcs.showFriend;
            break;
        case ADDFRIEND:
            work->func = funcs.addFriend;
            break;
        case FRIENDREQUEST:
            work->func = funcs.friend_req;
            break;
        case DELFRIEND:
            work->func = funcs.delFriend;
            break;
        case CREATGROUP:
            work->func = funcs.creatGroup;
            break;
        case ADDMEMBER:
            work->func = funcs.addMember;
            break;
        case KICKMEMBER:
            work->func = funcs.kickMember;
            break;
        case ADDGROUP:
            work->func = funcs.addGroup;
            break;
        case GROUPREQUEST:
            work->func = funcs.group_req;
            break;
        case SHOWGROUP:
            work->func = funcs.showGroup;
            break;
        case GROUPCHAT:
            work->func = funcs.groupChat;
            break;
        case ADDADMIN:
            work->func = funcs.addAdmin;
            break;
        case DELADMIN:
            work->func = funcs.delAdmin;
            break;
        default:;
    }
    std::cout << m << std::endl;

    return work;
}


#endif
