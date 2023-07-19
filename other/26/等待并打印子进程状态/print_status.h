#include <stdio.h>
#include <sys/wait.h>


void print_status(char* msg,int status)
{
    if (msg != NULL)
        printf("%s  ",msg);
    if (WIFEXITED(status))
        printf("normal exited on status %d\n",WEXITSTATUS(status));
    else if(WIFSIGNALED(status)){
        printf("signal killed by %d\n",WTERMSIG(status));
        if(WCOREDUMP(status))
            printf("create core\n");
    }
    else if(WIFSTOPPED(status))
        printf("child stop\n");
    else if(WIFCONTINUED(status))
        printf("child continue\n");
}
