#ifndef MYERROR
#define MYERROR
#include <stdio.h>
#include <stdlib.h>
void myerr(const char * msg)
{
    perror(msg);
    char* s = getenv("EF_DUMPCORE");
    if (s!=NULL && *s!='\0')
        abort();
    else
        exit(EXIT_FAILURE);
}

#endif
