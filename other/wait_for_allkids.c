#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "myerror.h"



int main(int argc,char *argv[])
{
    setbuf(stdout,NULL);

    for(int i=1;i<argc;i++)
    {
        switch(fork()){
            case -1:
                myerr("fork");
                break;
            case 0:
                printf("child %d sleep for %s seconds\n",getpid(),argv[i]);
                sleep(*argv[i] - '0');
                _exit(EXIT_SUCCESS);
            default:
                break;
        }
    }

    //father wait here for all his kids
    while(1)
    {
        if (wait(NULL)==-1)
        {
            if (errno == ECHILD)
            {
                printf("wall kids have been here!");
                exit(EXIT_SUCCESS);
            }
            else
                myerr("wait");
        }
    }

    return 0;
}
