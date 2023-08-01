#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <sys/epoll.h>
#include "srMsg.h"
#include "socket.hpp"
#include "menu.hpp"
#include "user.hpp"

#define MAXEVENTS 20

int sfd;



struct Msg{
    int flag;
    uint32_t uid;
    std::string password;

    Msg(std::string info)
    {
        nlohmann::json j = nlohmann::json::parse(info);
        flag     =   j["flag"];
        uid      =   j["uid"];
        password =   j["password"];
    }
    Msg() = default;
    nlohmann::json toJson()
    {
        nlohmann::json j;
        j["flag"]     =   flag;
        j["uid"]      =   uid;
        j["password"] =   password;

        return j;
    }
    std::string toStr()
    {
        return this->toJson().dump();
    }
};


void sendone(int sfd)
{
    std::string msg;
    std::cin >> msg;
    sendMsg(sfd,msg.c_str());
}
std::string readone(int fd)
{
    std::string msg;
    msg = readMsg(fd);
    std::cout << msg << std::endl;
    return msg;
}

void cliLog(int fd)
{
    Msg msg;
    std::cout << "Please input UID" << std::endl;
    scanf("%u",&msg.uid);
    std::cout << "Please input your password" << std::endl;
    std::cin >> msg.password;

    sendMsg(fd,msg.toStr().c_str());

}

void do_read(void)
{
    std::string rev = readMsg(sfd);


}

void do_epoll(void*)
{
    struct epoll_event ev;
    int epfd = epoll_create(3);

    ev.data.fd = sfd;
    ev.events  = EPOLLIN;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&ev);
    while (1)
    {
        epoll_wait(epfd,&ev,MAXEVENTS,-1);
        do_read();
    }

    close(epfd);
}


int main(int argc,char* argv[])
{
    sfd = inetConnect("127.0.0.1","7679");
    std::cout << pre_login_content << std::endl;











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
