#ifndef MYSOCKET
#define MYSOCKET

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "myerror.hpp"

#define BACKLOG 20

int inetListen(const char* service)
{
    struct addrinfo hints;
    struct addrinfo *result,*rp;
    memset(&hints,0,sizeof(struct addrinfo));

    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;
    hints.ai_flags     = AI_PASSIVE;
    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;

    getaddrinfo(NULL,service,&hints,&result);

    int sfd,optval;
    for (rp=result ; rp!=NULL ; rp=rp->ai_next)
    {
        sfd = socket(rp->ai_family,rp->ai_socktype,0);
        if (sfd==-1)
            continue; //look for another

        if (setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval))==-1){
            close(sfd);
            freeaddrinfo(result);
            return -1;
        }

        if(bind(sfd,rp->ai_addr,rp->ai_addrlen)==0)
            break;  //well done!
        close(sfd);
    }

    if (rp!= NULL)
        if (listen(sfd,BACKLOG)==-1){
            freeaddrinfo(result);
            myerr("listen");
        }

    return (rp==NULL)?-1: sfd;
}

int inetConnect(const char* host,const char* service)
{
    struct addrinfo hints;
    struct addrinfo *result,*rp;

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;
    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;

    int sfd,s;
    getaddrinfo(host,service,&hints,&result);

    for (rp=result; rp != NULL ; rp=rp->ai_next)
    {
        sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        if (sfd==-1){
            continue;
        }

        if (connect(sfd,rp->ai_addr,rp->ai_addrlen)==0)
            break;
        close(sfd);
    }
    freeaddrinfo(result);

    return (rp==NULL)?-1 :sfd;
}


#endif
