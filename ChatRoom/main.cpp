#include <sys/epoll.h>
#include <hiredis/hiredis.h>
#include "socket.hpp"
#include "srMsg.h"
#include "command.hpp"

#define PORT 7679
#define MAX_EVENTS 50
#define TOTTASKS 100
#define TOTTHREADS 20

int main(void)
{
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

                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
            }
            else
            {
                int fd = evlist[i].data.fd;
                ev.events = EPOLLIN;
                ev.data.fd = fd;
                while (1)
                {
                    Hred red(c);
                    Command command(fd,c);
                    std::unique_ptr<TASK> task = command.parse_command();
                    pool.add_task(std::move(task));

                    //redisReply *reply = red.get(3783);
                    //std::string s = reply->str;
                    //User user(s);
                    //std::cout << user.password << std::endl;
                }

                close(fd);
                epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);
            }
        }
    }

    return 0;
}
