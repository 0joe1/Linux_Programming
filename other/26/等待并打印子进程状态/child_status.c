#include <sys/wait.h>
#include <unistd.h>
#include "myerror.h"
#include "print_status.h"

int main(int argc,char* argv[])
{
    int status;
    int child_pid;

    switch(fork())
    {
        case -1:
            myerr("fork");
        case 0:
            child_pid=getpid();
            printf("child(ID=%d) start\n",child_pid);
            if (argc>1)
            {
                status = *argv[1]-'0';
                printf("child %d exit on status %d\n",getpid(),status);
                _exit(status);
            }
            else
            {
                while(1){
                    pause();
                }
                printf("child %d normal exit",getpid());
                _exit(EXIT_SUCCESS);
            }
            break;
        default:
            while(1){
                child_pid=waitpid(-1,&status,WUNTRACED|WCONTINUED);
                if ((status&0xFF)==0)
                    printf("waitpid() returned:PID=%d ,exit on status=%d\n",child_pid,status>>8);
                print_status(NULL,status);

                if(WIFEXITED(status)|WIFSIGNALED(status))
                    _exit(EXIT_SUCCESS);
            }
    }
}
