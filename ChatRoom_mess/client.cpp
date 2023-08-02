#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/epoll.h>
#include "menu.hpp"
#include "socket.hpp"
#include "srMsg.hpp"
#include "user.hpp"

#define MAXEVENTS 20


int sfd;
bool islog;
int myid;
int talkto=-1;


enum tasks {
    LOGIN,
    SIGNUP,
    FRIENDCHAT,
    ADDFRIEND,
    FRIENDREQUEST
};

pthread_mutex_t mlog = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  colog = PTHREAD_COND_INITIALIZER;


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
    myid = msg.uid;
    std::cout << "Please input your password" << std::endl;
    std::cin >> msg.password;

    sendMsg(fd, msg.toStr().c_str());
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
}
void cliLog(int fd) {
    Msg msg;
    msg.flag = 0;
    std::cout << "Please input UID" << std::endl;
    scanf("%u", &msg.uid);
    myid = msg.uid;
    std::cout << "Please input your password" << std::endl;
    std::cin >> msg.password;

    sendMsg(fd, msg.toStr().c_str());
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
}

void friChat(int fd)
{
    Msg msg;
    msg.flag = 2;

    msg.uid = myid;
    std::cout << "请输入您想聊天的用户的UID" << std::endl;
    scanf("%u",&msg.touid);
    talkto = msg.touid;

    printf("--------与用户%d的聊天(按Q退出)-----------",talkto);
    std::string content;
    std::cin >> content;
    while (content != "Q")
    {
        msg.content = content;
        sendMsg(fd,msg.toStr().c_str());
        std::cin >> content;
    }
}

void addFriend(int fd)
{
    Msg msg;
    msg.flag = ADDFRIEND;

    msg.uid = myid;
    std::cout << "请选择您想要的伙伴(uid)" << std::endl;
    scanf("%u",&msg.touid);

    sendMsg(fd,msg.toStr().c_str());
}

void print_message(std::string buf)
{
    pthread_mutex_lock(&mlog);

    std::cout << buf << std::endl;
    if (buf == "登陆成功" || buf == "注册成功"){
        islog = 1;
    }

    pthread_cond_signal(&colog);
    pthread_mutex_unlock(&mlog);
}

void prv_recv(std::string buf)
{
    std::cout << buf << std::endl;
}

void friend_req(std::string buf,int fd)
{
    Msg msg;
    msg.flag = FRIENDREQUEST;
    // know who send me request
    uint32_t fuid = std::stoul(buf);

    std::string choice;
    std::cout << std::to_string(fuid) + " wants you! Accept?(y/n)" << std::endl;
    std::cin >> choice;
    std::cout << choice << std::endl;
    while (choice != "y" && choice != "n"){
        std::cout << choice << std::endl;
        std::cout << "Please input y or n" << std::endl;
        std::cin >> choice;
    }

    msg.uid     =  myid;
    msg.touid   =  fuid;
    msg.content =  choice;
    sendMsg(fd,msg.toStr().c_str());
}

void do_read(int fd)
{
    std::string t = readMsg(fd);
    rMsg rmg(t);

    int choice = rmg.flag;
    std::string str = rmg.mg;
    switch(choice)
    {
        case 0:
        case 1:
        case FRIENDREQUEST:
            std::cout << choice << std::endl;
            print_message(rmg.mg);
            break;
        case 2:
            std::cout << choice << std::endl;
            prv_recv(rmg.mg);
            break;
        case ADDFRIEND:
            std::cout << choice << std::endl;
            friend_req(rmg.mg,fd);
            break;

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

        std::cin >> choice;

        switch(choice)
        {
            case LOGIN:
                cliLog(sfd);
                break;
            case SIGNUP:
                cliSign(sfd);
            case FRIENDCHAT:
                friChat(sfd);
                break;
            case ADDFRIEND:
                addFriend(sfd);
                break;
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
    sleep(7);











    /*
    //第一次给epoll解锁，被parse_command接受作为选择
    sendone(sfd);
    //接收 login 的 Please input UID,然后传输uid
    readone(sfd);
    sendone(sfd);
    //Please input Password
    readone(sfd);
    sendone(sfd);
    //登陆成功
    readone(sfd);
    std::cout << after_login_content << std::endl;

    std::string t;
    std::cin >>t;
    if (t=="6"){
        while (1){
            readone(sfd);
        }
    }
    //parse_command 参数
    sendone(sfd);
    std::cout << "请输入您想聊天的用户的UID" << std::endl;

    std::string msg;
    */

    return 0;
}
