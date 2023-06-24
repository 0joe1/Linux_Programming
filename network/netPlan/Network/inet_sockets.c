#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "inet_sockets.h"

int inetConnect(const char*host,const char *service,int type)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd;

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_canonname=NULL;
    hints.ai_addr=NULL;
    hints.ai_next=NULL;
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype = type;

    getaddrinfo(host,service,&hints,&result);

    while (rp=result;rp != NULL;rp=rp->ai_next)
    {
        sfd = socket(rp->ai_family,rp->ai_socktype,0);
        if (sfd==-1)
            continue;

        if (connect(sfd,rp->ai_addr,rp->ai_addrlen) != -1)
            break;
        close(sfd);
    }
    freeaddrinfo(result);

    return (rp==NULL)?-1:sfd;
}
