#ifndef SRMSG
#define SRMSG

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <nlohmann/json.hpp>

#include "myerror.hpp"


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

struct Msg {
    int flag;
    uint32_t uid;
    uint32_t touid{0};
    uint32_t adduid{0};
    std::string content;
    std::string password;

    Msg(std::string info) {
        std::cout << info << std::endl;
        nlohmann::json j = nlohmann::json::parse(info);
        flag     = j["flag"];
        uid      = j["uid"];
        touid    = j["touid"];
        adduid   = j["adduid"];
        content  = j["content"];
        password = j["password"];
    }
    Msg() = default;
    nlohmann::json toJson() {
        nlohmann::json j;
        j["flag"]     = flag;
        j["uid"]      = uid;
        j["touid"]    = touid;
        j["adduid"]   = adduid;
        j["content"]  = content;
        j["password"] = password;

        return j;
    }
    std::string toStr() { return this->toJson().dump(); }
};

struct rMsg{
    int flag;
    std::string mg;

    rMsg(std::string info) {
        nlohmann::json j = nlohmann::json::parse(info);
        flag = j["flag"];
        mg  = j["mg"];
    }
    rMsg() = default;
    nlohmann::json toJson(){
        nlohmann::json j;
        j["flag"] = flag;
        j["mg"] = mg;
        return j;
    }
    std::string toStr() { return this->toJson().dump(); }
};

struct chatMsg{
    uint32_t fid;
    uint32_t gid{0};
    std::string content;

    chatMsg(std::string info){
        nlohmann::json j = nlohmann::json::parse(info);
        fid     =  j["fid"];
        gid     =  j["gid"];
        content =  j["content"];
    }
    chatMsg() = default;
    nlohmann::json toJson(){
        nlohmann::json j;
        j["fid"] = fid;
        j["gid"] = gid;
        j["content"] = content;
        return j;
    }
    std::string toStr() { return this->toJson().dump(); }
};

struct fileMsg{
    uint32_t sender;
    uint32_t receiver;
    std::string content;

    fileMsg(uint32_t sender_,uint32_t receiver_):
        sender(sender_),receiver(receiver_){}
    fileMsg(std::string info){
        nlohmann::json j = nlohmann::json::parse(info);
        sender   =  j["sender"];
        receiver =  j["receiver"];
        content  =  j["content"];
    }
    fileMsg() = default;
    nlohmann::json toJson(){
        nlohmann::json j;
        j["sender"]    =  sender;
        j["receiver"]  =  receiver;
        j["content"]   =  content;
        return j;
    }
    std::string toStr() { return this->toJson().dump(); }
};

struct groupReq{
    uint32_t uid;
    uint32_t gid;
    int      status=0;

    groupReq(uint32_t tuid,uint32_t tgid):
        uid(tuid),gid(tgid){}
    groupReq(std::string info){
        std::cout << info << std::endl;
        nlohmann::json j = nlohmann::json::parse(info);
        uid     =  j["uid"];
        gid     =  j["gid"];
        status  =  j["status"];
    }
    groupReq() = default;
    nlohmann::json toJson(){
        nlohmann::json j;
        j["uid"]    =  uid;
        j["gid"]    =  gid;
        j["status"] =  status;
        return j;
    }
    std::string toStr() { return this->toJson().dump(); }
};




void sendmg(int fd,rMsg *msg,std::string mg)
{
    msg->mg = mg;
    sendMsg(fd,msg->toStr().c_str());
}


#endif
