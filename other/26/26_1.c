#include <stdio.h>
#include <unistd.h>
#include "myerror.h"

int main(void)
{
    switch(fork())
    {
        case -1:
            myerr("fork");
        case 0:
            sleep(3);
            printf("who is my daddy?\n");
            printf("%ld\n",(long)getppid());
            _exit(EXIT_SUCCESS);
        default:
            printf("Father %d existing\n",getpid());
            printf("Now the father kills himself\n");
            fflush(stdout);
            _exit(EXIT_SUCCESS);
    }

    return 0;
}
