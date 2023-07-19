#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "myerror.h"

static volatile int numLiveChildren;

void kill_zombies(int sig)
{
    int savedErrno = errno;
    pid_t child_pid;
    if(sig==SIGCHLD)
        printf("caught SIGCHLD\n");
    else
        myerr("unexpected signal");

    while((child_pid=waitpid(-1,NULL,WNOHANG))>0){
        numLiveChildren--;
        printf("Kill child(zombie) %d \n",child_pid);
    }
    if (child_pid==-1 && errno!=ECHILD)
        myerr("wait");

    sleep(5);
    printf("Now have %d children\n",numLiveChildren);
    printf("numLiveChildren:%d\n",numLiveChildren);

    errno = savedErrno;
}


int main(int argc,char* argv[])
{
    numLiveChildren = argc-1;
    setbuf(stdout,NULL);

    sigset_t blockset,emptyset;
    sigemptyset(&blockset);
    sigaddset(&blockset,SIGCHLD);
    sigprocmask(SIG_SETMASK,&blockset,NULL);

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = kill_zombies;
    sigaction(SIGCHLD,&sa,NULL);

    for (int i=1;i<argc;i++)
    {
        switch(fork())
        {
            int sleeptime;
            case -1:
                myerr("fork");
            case 0:
                printf("Child %d existing......\n",getpid());
                sleeptime = *argv[i]-'0';
                sleep(sleeptime);
                _exit(EXIT_SUCCESS);
            default:;
        }
    }

    sigemptyset(&emptyset);
    while (numLiveChildren > 0)
    {
        sigsuspend(&emptyset);
    }
}
