#include <stdio.h>
#include <signal.h>
#include <string.h>
#define CMDSIZE 100
#define MAXARGS 10

char whitespace[]="\t\r\n\v";
char symbols[] = "<|>&;()";

void peek(char**ps,char*es,char*toks)
{
    char*s=*ps;
    while (s<es && strchr(whitespace,*s))
        s++;
    *ps=s;
    return *s && strchr(toks,*s); 
}

char gettoken(char**ps,char*es,char*content)
{
    char* p= ps;
    peek(ps,es,"");
    if (p<es && strchr())
    {
    
    }

    while(p<es && strchr(symbols,*p))
        p++;
}



void getcmd(char* buf);
void analyze(char* buf);

struct cmd{
    int type;
};
struct execcmd{
    int type;
    char* arglt[MAXARGS];
};
struct redircmd{
    int type;
    int oldfd;
    int mode;
    char newfile[200];
    struct cmd* cmd;
};
struct pipecmd{
    int type;
    struct cmd* left;
    struct cmd* right;
};
struct listcmd{
    int type;
    struct cmd* left;
    struct cmd* right;
};


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
    char*p,*q,*es;
    *p=*q=buf;
    *es=p+strlen(buf);

    peek(p,es,""); // skip the whitespace
    





    /*
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
    */

}
