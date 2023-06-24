#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H

#include "myerror.h"
#include <sys/socket.h>
#include <netdb.h>

int inetConnect(const char* host,const char*service,int type);




#endif
