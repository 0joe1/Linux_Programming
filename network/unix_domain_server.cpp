#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100

using namespace std;


class Server{
private:
    struct sockaddr_un addr;
    struct sockaddr_un rec_from;
    int sfd;
    int backlog;
    size_t buf_size;

public:
    Server(string path,int b,size_t b_s)
        :backlog(b),buf_size(b_s)
    {
        memset(&addr,0,sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path,path.c_str(),sizeof(addr.sun_path)-1);
        sfd = socket(AF_UNIX,SOCK_STREAM,0);
    }
    void start(void){
        remove(addr.sun_path);
        bind(sfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un));
        listen(sfd,backlog);
    }
    void working(void);
};

void Server::working()
{
    while(1)
    {
        int cfd;
        socklen_t len = sizeof(struct sockaddr_un);
        cfd = accept(sfd,(struct sockaddr*)&rec_from,&len);

        ssize_t numRead=0;
        char buf[buf_size];
        while((numRead = read(cfd,buf,buf_size)))
        {
            write(STDOUT_FILENO,buf,numRead);
        }
        cout << "received from "<<rec_from.sun_path <<endl;

        close(cfd);
    }
}



int main(void)
{
    return 0;
}

