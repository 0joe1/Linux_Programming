#ifndef SRMSG
#define SRMSG

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "myerror.h"


ssize_t writen(int fd,const char* msg,int n)
{
    ssize_t numWritten;
    size_t totWritten=0;
    const char* buf=msg;
    while (totWritten < n)
    {
        numWritten = write(fd,buf,n-totWritten);
        if (numWritten<0){
            if (numWritten==-1 && errno==EINTR)
                continue;
            else{
                printf("here");
                return -1;
            }
        }

        totWritten += numWritten;
        buf += numWritten;
    }

    return totWritten;
}

ssize_t readn(int fd,char* buffer,int n)
{
    char* buf = buffer;
    ssize_t numRead;
    size_t totRead=0;
    while (totRead < n)
    {
        numRead = read(fd,buf,n-totRead);
        if (numRead==0){
            return totRead;
        }
        if (numRead==-1){
            if (errno==EINTR)
                continue;
            else
                return -1;
        }

        totRead += numRead;
        buf += numRead;
    }
    return totRead;
}

void sendMsg(int fd,const char* msg)
{
    int size = strlen(msg);
    uint32_t sendsize = htonl(size);

    writen(fd,(char*)&sendsize,4);
    writen(fd,msg,size);
}
char* readMsg(int fd)
{
    int size;
    readn(fd,(char*)&size,4);
    size = ntohl(size);

    char* buf = (char*)malloc(sizeof(char)*(size+1));
    readn(fd,buf,size);
    buf[size]='\0';

    return buf;
}


#endif
