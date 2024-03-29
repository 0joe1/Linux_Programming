#include <sys/epoll.h>
#include <hiredis/hiredis.h>
#include <signal.h>
#include <sys/stat.h>
#include "../Common/socket.hpp"
#include "../Common/srMsg.hpp"
#include "command.hpp"

#define PORT 7679
#define MAX_EVENTS 50
#define TOTTASKS 100
#define TOTTHREADS 20

std::map<uint32_t,int> fdMap;

const char* server_files       = "./server_files/";

int main(void)
{
    sigset_t blockset;
    sigemptyset(&blockset);
    sigaddset(&blockset,SIGPIPE);
    sigprocmask(SIG_SETMASK,&blockset,NULL);

    struct stat st;
    if (stat(server_files, &st) == -1) {
      // 目录不存在,调用mkdir创建
      mkdir(server_files, 0755);
    }

    thread_pool pool(TOTTASKS,TOTTHREADS);
    redisContext *c = redisConnect("127.0.0.1",6379);
    if (c==NULL || c->err)
    {
        if (c)
            printf("Error:%s\n",c->errstr);
        else
            printf("Can't allocate redis context\n");
    }


    int lfd,cfd;
    lfd = inetListen(std::to_string(PORT).c_str());
    if (lfd==-1)
        myerr("inetListen");

    int epfd,ready;
    struct epoll_event lev;
    struct epoll_event evlist[MAX_EVENTS];
    epfd = epoll_create(20);
    if (epfd==-1)
        myerr("epoll_create");
    lev.data.fd = lfd;
    lev.events = EPOLLIN;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&lev);


    std::string msg;
    while(1)
    {
        std::cout << "waiting..." << std::endl;
        ready = epoll_wait(epfd,evlist,MAX_EVENTS,-1);
        if (ready == -1)
            myerr("epoll_wait");
        std::cout << ready << std::endl;
        for (int i=0;i<ready;i++)
        {
            struct epoll_event ev;
            if (evlist[i].data.fd == lfd)
            {
                cfd = accept(lfd,NULL,0);

                //设置非阻塞
                int flags = fcntl(cfd,F_GETFL);
                fcntl(cfd,F_SETFL,flags | O_NONBLOCK);

                ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
                ev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
                std::cout << "Press any key to start" << std::endl;
            }
            else if (evlist[i].events & (EPOLLHUP | EPOLLRDHUP))
            {
                std::cout << "EPOLLRDHUP" << std::endl;
                epoll_ctl(epfd,EPOLL_CTL_DEL,evlist[i].data.fd,&ev);
                close(evlist[i].data.fd);
                for (auto it = fdMap.begin() ; it != fdMap.end() ; it++)
                {
                    if (it->second == evlist[i].data.fd){
                        fdMap.erase(it->first);
                        std::cout << "erase " << std::endl;
                    }
                }
            }
            else
            {
                int fd = evlist[i].data.fd;
                ev.events = EPOLLIN;
                ev.data.fd = fd;

                Command *command = new Command(fd,c,&pool,epfd);
                unique_ptr<TASK> cmd = std::move(command->parse_command());
                if (cmd.get() == nullptr)
                    continue;
                epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);
                pool.add_task(std::move(cmd));
            }
        }
    }

    return 0;
}
