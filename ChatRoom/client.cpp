#include <iostream>
#include <string>
#include "srMsg.h"
#include "socket.hpp"


int main(int argc,char* argv[])
{
    int sfd;
    sfd = inetConnect("127.0.0.1","7679");
    while(1)
    {
        std::string msg;
        std::cin >> msg;
        sendMsg(sfd,msg.c_str());
    }

    return 0;
}
