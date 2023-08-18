#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../Common/myerror.hpp"

#define BUFFERSIZE 512

void deleteBefore(int* fd,const char* filename)
{
    int tmp_fd;
    ssize_t bread = 0;
    char buffer[BUFFERSIZE];

    if ((tmp_fd = open("tmp.txt",O_RDWR | O_CREAT ,0600)) == -1){
        myerr("open tmp");
    }

    while ((bread = read(*fd,buffer,BUFFERSIZE)) != 0)
    {
        if(bread == -1){
            myerr("read");
        }

        if (write(tmp_fd,buffer,bread) == -1){
            myerr("write");
        }
    }
    lseek(tmp_fd,0,SEEK_SET);

    close(*fd);
    if (remove(filename) == -1){
        myerr("remove");
    }
    rename("tmp",filename);
    *fd = tmp_fd;
}
