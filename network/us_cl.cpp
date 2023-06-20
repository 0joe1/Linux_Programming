#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/un.h>
#include <sys/socket.h>

#define SV_SOCK_PATH "tmp/us_xfr"

class Client{
private:
    int sfd;
    size_t buf_size;
public:
    Client(size_t b_s)
        :buf_size(b_s)
    {
        sfd = socket(AF_UNIX,SOCK_STREAM,0);
    }
    void connecting(std::string sv_path);
    void working();
};

void Client::connecting(std::string sv_path)
{
    struct sockaddr_un sv_addr;
    memset(&sv_addr,0,sizeof(struct sockaddr_un));
    sv_addr.sun_family = AF_UNIX;
    strncpy(sv_addr.sun_path,sv_path.c_str(),sizeof(sv_addr.sun_path)-1);

    connect(sfd,(struct sockaddr*)&sv_addr,sizeof(struct sockaddr_un));
}

void Client::working()
{
    ssize_t numRead;
    char buf[buf_size];
    while ((numRead = read(STDIN_FILENO,buf,buf_size))>0)
        write(sfd,buf,numRead);
}



int main(int argc,char* argv[])
{
    return 0;
}
