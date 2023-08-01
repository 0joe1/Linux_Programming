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

pthread_mutex_t mlog = PTHREAD_MUTEX_INITIALIZER;
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
    std::cout << "Please input your password" << std::endl;
    std::cin >> msg.password;

    sendMsg(fd, msg.toStr().c_str());
    islog = 1;
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
}
void cliLog(int fd) {
    Msg msg;
    msg.flag = 0;
    std::cout << "Please input UID" << std::endl;
    scanf("%u", &msg.uid);
    std::cout << "Please input your password" << std::endl;
    std::cin >> msg.password;

    sendMsg(fd, msg.toStr().c_str());
    islog = 1;
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
}

void print_message(std::string buf)
{
    pthread_mutex_lock(&mlog);

    std::cout << buf << std::endl;

    pthread_cond_signal(&colog);
    pthread_mutex_unlock(&mlog);
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
            std::cout << choice << std::endl;
            print_message(rmg.mg);
    }
}

void* do_epoll(void *) {
    struct epoll_event ev;
    struct epoll_event evlist[MAXEVENTS];
    int epfd = epoll_create(3);

    ev.data.fd = sfd;
    ev.events = EPOLLIN;
    std::cout << "test1" << std::endl;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);
    std::cout << "test2" << std::endl;
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
            case 0:
                cliLog(sfd);
                break;
            case 1:
                cliSign(sfd);
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
