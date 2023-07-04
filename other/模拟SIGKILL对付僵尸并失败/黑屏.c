#include <stdio.h>
#include <signal.h>

int main(void)
{
    kill(-1,SIGKILL);
    return 0;
}
