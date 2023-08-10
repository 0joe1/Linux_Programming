#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include "menu.hpp"
#include "socket.hpp"
#include "srMsg.hpp"
#include "user.hpp"
#include "fileio.hpp"

#define MAXEVENTS 20


const char* blank = "                                   ";
const char* friend_requests = "friend_requests.txt";
const char* group_requests  = "group_requests.txt";
const char* prvchat_message = "prvchat_message.txt";

int sfd;
bool islog;
uint32_t myid;
uint32_t talkto;


enum tasks {
    LOGIN,
    SIGNUP,
    FRIENDCHAT,
    SHOWFRIEND,
    ADDFRIEND,
    DELFRIEND,
    FRIENDREQUEST,
    CREATGROUP,
    ADDMEMBER,
    ADDGROUP,
    GROUPREQUEST
};

pthread_mutex_t mlog  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx   = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  colog = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cm    = PTHREAD_COND_INITIALIZER;


bool isNumeric(std::string const &str)
{
    auto it = str.begin();
    while (it != str.end() && std::isdigit(*it)) {
        it++;
    }
    return !str.empty() && it == str.end();
}

void sendone(int sfd) {
    std::string msg;
    std::cin >> msg;
    sendMsg(sfd, msg.c_str());
}
std::string readone(int fd) {
    std::string msg;
    msg = readMsg(fd);
    std::cout << msg << std::endl;
    return msg;
}

void cliSign(int fd) {
    Msg msg;
    msg.flag = 1;
    std::cout << "Please input UID" << std::endl;
    scanf("%u", &msg.uid);
    while (msg.uid <= 0){
        std::cout << "uid 必须大于0" << std::endl;
        scanf("%u", &msg.uid);
    }
    myid = msg.uid;
    std::cout << "Please input your password" << std::endl;
    getchar();
    getline(std::cin,msg.password);

    sendMsg(fd, msg.toStr().c_str());
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
    pthread_mutex_unlock(&mlog);
}
void cliLog(int fd) {
    Msg msg;
    msg.flag = 0;
    std::cout << "Please input UID" << std::endl;
    scanf("%u", &msg.uid);
    getchar();
    myid = msg.uid;

    std::cout << "Please input your password" << std::endl;
    getline(std::cin,msg.password);

    sendMsg(fd, msg.toStr().c_str());
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
    pthread_mutex_unlock(&mlog);
}

void load_prvchat_message(uint32_t id)
{
    std::vector<chatMsg> messages;
    std::string filename = std::to_string(myid)+prvchat_message;
    std::ifstream file(filename);
    std::ofstream tempFile("temp_prvchat_message.txt");
    if (file.is_open())
    {
        std::string line;
        while (getline(file,line))
        {
            chatMsg msg;
            std::istringstream record(line);
            record >> msg.fid >> msg.content;
            if (msg.fid == id)
                std::cout << msg.content << std::endl;
            else
                tempFile << line << std::endl;
        }
    }
    file.close();
    tempFile.close();
    std::remove(prvchat_message);
    std::rename("temp_prvchat_message.txt",filename.c_str());
}

void friChat(int fd)
{
    Msg msg;
    msg.flag = 2;

    msg.uid = myid;
    std::cout << "请输入您想聊天的用户的UID" << std::endl;
    scanf("%u",&msg.touid);
    while (msg.uid <= 0){
        std::cout << "请输入大于0的uid号" << std::endl;
        scanf("%u",&msg.touid);
    }
    talkto = msg.touid;

    printf("--------与用户%d的聊天(按Q退出)-----------",talkto);
    load_prvchat_message(talkto);
    std::string content;
    std::cin >> content;
    while (content != "Q")
    {
        msg.content = content;
        sendMsg(fd,msg.toStr().c_str());
        std::cin >> content;
    }
    talkto = 0;
}

void showFriend(int fd)
{
    Msg msg;
    msg.flag = SHOWFRIEND;

    msg.uid = myid;
    sendMsg(fd,msg.toStr().c_str());
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cm,&mtx);
    pthread_mutex_unlock(&mtx);
}

void addFriend(int fd)
{
    Msg msg;
    msg.flag = ADDFRIEND;

    msg.uid = myid;
    std::cout << "请选择您想要的伙伴(uid)" << std::endl;
    scanf("%u",&msg.touid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void friend_req(int fd)
{
    Msg msg;
    msg.flag = FRIENDREQUEST;

    int reqfd;
    if ((reqfd = open(friend_requests,O_RDONLY))==-1){
        if (errno == ENOENT){
            std::cout << "暂时还没有好友请求" << std::endl;
            return;
        }
        else
            myerr("open friend_requests");
    }

    char* buffer = (char*)malloc(sizeof(uint32_t));

    read(reqfd,buffer,sizeof(uint32_t));

    uint32_t fuid = strtoul(buffer,NULL,10);
    std::string request = std::to_string(fuid) + " wants you! Accept?(y/n)";
    std::cout << request << std::endl;
    std::string choice;
    getline(std::cin,choice);
    while (choice != "y" && choice != "n"){
        std::cout << "Please input y or n" << std::endl;
        getline(std::cin,choice);
    }

    msg.uid     =  myid;
    msg.touid   =  fuid;
    msg.content =  choice;
    sendMsg(fd,msg.toStr().c_str());

    deleteBefore(&reqfd,friend_requests);
}
void group_req(int fd)
{

}

void delfriend(int fd)
{
    Msg msg;
    msg.flag = DELFRIEND;

    uint32_t touid;
    std::cout << "想删谁？" << std::endl;
    scanf("%u",&touid);
    getchar();

    msg.uid   = myid;
    msg.touid = touid;
    sendMsg(fd,msg.toStr().c_str());
}

void createGroup(int fd)
{
    Msg msg;
    msg.flag = CREATGROUP;

    msg.uid = myid;
    std::cout << "请输入您想创建的群聊的id" << std::endl;
    scanf("%u", &msg.touid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void addMember(int fd)
{
    Msg msg;
    msg.flag = ADDMEMBER;

    msg.uid = myid;
    std::cout << "想为哪个群添加新成员呢？（输入groupid)" << std::endl;
    scanf("%u",&msg.touid);
    std::cout << "输入想拉入的新成员的id" << std::endl;
    scanf("%u",&msg.adduid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void addGroup(int fd)
{
    Msg msg;
    msg.flag = ADDGROUP;

    msg.touid = myid;
    std::cout << "请输入您想加入的群聊id" << std::endl;
    scanf("%u",&msg.uid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}


void print_message(std::string buf)
{
    pthread_mutex_lock(&mlog);

    std::cout << "print message" << std::endl;
    std::cout << buf << std::endl;
    if (buf == "登陆成功" || buf == "注册成功"){
        islog = 1;
    }

    pthread_mutex_unlock(&mlog);
    pthread_cond_signal(&colog);
}

void save_prvchat_message(chatMsg chatmsg)
{
    std::string filename = std::to_string(myid)+prvchat_message;
    std::ofstream file(filename,std::ios::app);

    std::string content = std::to_string(chatmsg.fid)+" "+chatmsg.content;
    if (file.is_open())
    {
        std::cout << blank << chatmsg.fid << "send you a message" << std::endl;
        std::cout << content << std::endl;
        file << content << std::endl;
    }
    else
    {
        std::cout << "failed to open the file" << std::endl;
    }
}

void prv_recv(std::string buf)
{
    chatMsg chatmsg(buf);
    if (chatmsg.fid == talkto){
        std::cout << chatmsg.fid << ":"<< chatmsg.content << std::endl;
    }
    else
    {
        save_prvchat_message(chatmsg);
    }
}

void save_friend_request(std::string buf)
{
    if (!isNumeric(buf)){
        std::cout << buf << std::endl;
        return;
    }

    // know who send me request
    uint32_t fuid = std::stoul(buf);

    int fd;
    if ((fd = open("friend_requests.txt",O_WRONLY | O_APPEND | O_CREAT,0600)) == -1){
        myerr("open friend_requests");
    }
    int ret = write(fd,std::to_string(fuid).c_str(),sizeof(uint32_t));
    if (ret == -1){
        myerr("write friend_requests");
    }

    close(fd);
}
void friendShow(std::string buf)
{
    struct FriendInfo {
        uint32_t  frid;
        int       online;
    };

    FriendInfo info;
    std::vector<FriendInfo> frv;
    std::istringstream record(buf);

    while (record >> info.frid >> info.online ){
        frv.push_back(info);
    }

    pthread_mutex_lock(&mtx);
    for (auto frd : frv)
    {
        const char* status;
        if (frd.online)
            status = "online";
        else
            status = "offline";
        printf("user:%d status:%s \n",frd.frid,status);
    }
    pthread_mutex_unlock(&mtx);
    pthread_cond_signal(&cm);
}

void save_group_request(std::string buf)
{
    groupReq gq(buf);
    std::string filename = group_requests + std::to_string(myid);
    std::string content  = std::to_string(gq.uid)+" "+std::to_string(gq.gid)+" "+std::to_string(gq.status);
    std::ofstream file(filename,std::ios::app);

    if (file.is_open())
    {
        file << content << std::endl;
    }
    else
    {
        std::cout << "failed to open the file" << std::endl;
    }
}


void do_read(int fd)
{
    std::string t = readMsg(fd);
    rMsg rmg(t);

    int choice = rmg.flag;
    std::string str = rmg.mg;
    switch(choice)
    {
        case LOGIN:
        case SIGNUP:
        case DELFRIEND:
        case CREATGROUP:
        case ADDMEMBER:
            std::cout << choice << std::endl;
            print_message(rmg.mg);
            break;
        case FRIENDCHAT:
            std::cout << choice << std::endl;
            prv_recv(rmg.mg);
            break;
        case ADDFRIEND:
            std::cout << choice << std::endl;
            save_friend_request(rmg.mg);
            break;
        case SHOWFRIEND:
            std::cout << choice << std::endl;
            friendShow(rmg.mg);
            break;
        case ADDGROUP:
            std::cout << choice << std::endl;
            save_group_request(rmg.mg);
            break;
        //case FRIENDREQUEST:
            //std::cout << choice << std::endl;
            //friendRequest(rmg.mg);
            //break;
        default:
            std::cout << "default" << std::endl;
    }
}

void* do_epoll(void *) {
    struct epoll_event ev;
    struct epoll_event evlist[MAXEVENTS];
    int epfd = epoll_create(3);

    ev.data.fd = sfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);
    while (1) {
        epoll_wait(epfd, evlist, MAXEVENTS, -1);
        do_read(sfd);
    }

    close(epfd);

    return NULL;
}

void mainDisplay(int sfd)
{
    int choice;
    while (1)
    {
        if (!islog)
            std::cout << pre_login_content << std::endl;
        else
            std::cout << after_login_content << std::endl;

        scanf("%d",&choice);
        getchar();

        switch(choice)
        {
            case LOGIN:
                cliLog(sfd);
                break;
            case SIGNUP:
                cliSign(sfd);
                break;
            case FRIENDCHAT:
                friChat(sfd);
                break;
            case SHOWFRIEND:
                showFriend(sfd);
                break;
            case ADDFRIEND:
                addFriend(sfd);
                break;
            case FRIENDREQUEST:
                friend_req(sfd);
                break;
            case DELFRIEND:
                delfriend(sfd);
                break;
            case CREATGROUP:
                createGroup(sfd);
                break;
            case ADDMEMBER:
                addMember(sfd);
                break;
            case ADDGROUP:
                addGroup(sfd);
                break;
            case GROUPREQUEST:
                group_req(sfd);
                break;
            default:;
        }
    }
}


int main(int argc, char *argv[]) {

    sfd = inetConnect("127.0.0.1", "7679");

    pthread_t thid;
    if (pthread_create(&thid,NULL,do_epoll,NULL) != 0){
        myerr("creat thread");
    }
    mainDisplay(sfd);


    return 0;
}
