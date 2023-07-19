#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <libgen.h>
#include "myerror.h"

#define MAX_CMDLEN 100

int main(int argc,char* argv[])
{
    char cmd[MAX_CMDLEN];
    pid_t child_pid;

    switch(child_pid=fork())
    {
        case -1:
            myerr("fork");
        case 0:
            printf("child %d existing......\n",getpid());
            fflush(NULL);
            _exit(EXIT_SUCCESS);
        default:
            sleep(2);
            snprintf(cmd,MAX_CMDLEN,"ps | grep %s",basename(argv[0]));
            printf("zombie:we are(I am) coming!!!\n");
            system(cmd);

            printf("Can I kill the zombie using my SIGKILL?\n");
            kill(child_pid,SIGKILL);
            system(cmd);
            wait(NULL);
    }
    printf("after wait\n");
    system(cmd);

    return 0;
}
