#include "fileio.hpp"

int main(void)
{
    //test rename
    char testbuf[1024]="test rename";
    int testfd;
    testfd = open("test_rename.txt",O_RDWR | O_CREAT,0777);
    if (write(testfd,testbuf,100) == -1){
        myerr("before rename");
    }
    rename("test_rename.txt","after_rename.txt");
    if (write(testfd,testbuf,100) == -1){
        myerr("before rename");
    }


    int fd;
    char buf[1024]= "hello-world";
    char nbuf[1024]="";
    char nnbuf[1024]="";
    fd = open("testio.txt",O_RDWR | O_CREAT,0777);
    printf("%d\n",fd);
    read(fd,nbuf,2);
    printf("%s\n",nbuf);
    deleteBefore(&fd,"testio.txt");
    printf("%d\n",fd);
    int a = read(fd,nnbuf,2);
    if (a==-1){
        myerr("read");
    }
    printf("%s\n",nnbuf);
    close(fd);
    return 0;
}
