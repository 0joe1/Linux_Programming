#include <stdio.h>
#include <signal.h>
#include <string.h>
#define CMDSIZE 100
#define MAXARGS 10


void getcmd(char* buf);
void analyze(char* buf);


struct execcmd{
    int type;
    char* arglt[MAXARGS];
}
struct redircmd{
    int type;

}


void makeexec()
{
    
}


int main(void)
{
    sigset_t blockmask;

    // block some signal
    sigemptyset(&blockmask);
    sigaddset(&blockmask,SIGINT);
    sigprocmask(SIG_SETMASK,&blockmask,NULL);

    
    return 0;
}

void getcmd(char* buf)
{
    printf("%s","$ ");
    fgets(buf,CMDSIZE,stdin);
    buf[strlen(buf)-1] = '\0';
    return;
}
void analyze(char* buf)
{
    char symbols[] = "<|>&;";
    char*p,*q,*es;
    *p=*q=buf;
    *es=p+strlen(buf);

    char* t;
    while (*p)
    {
        while (q < es && !strchr(symbols,*q))
            q++;
        switch(*q){
            case 0:makeexec();
                   break;
            case '>':make
        }

    }
}
