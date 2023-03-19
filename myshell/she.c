#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#define CMDSIZE 100
#define MAXARGS 10

#define EXEC 1
#define BACK 2
#define REDIR 3
#define PIPE 4 


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

void gettoken(char**ps,char*es,char**content)
{
    char* p= *ps;
    char* s;
    //char token;
    peek(p,es,"");
    
    switch(*p)
    {
        
        case 0:break;
        case '|':
        case '&':
        case '<':
        case ';':
        case '(':

        case ')':
               p++;
               break;
        case '>':
               p++;
               if (*p=='>')
               {
                   p++;
                   //token='+';
               }
               break;
        
    }
    peek(p,es,"");
    if (!content) return;
    s=p;
    while(p<es && !strchr(symbols,*p))
        p++;
    len=strlen(s)-strlen(p)+1;   //98s654p21'0' s-p:7-3+1=4+1=5

    char* cont = (char*)malloc(sizeof(char)*(len+1));
    snprintf(cont,len+1,"%s",s);
    *content=cont;
}



void getcmd(char* buf);
void analyze(char* buf);
void parseline(char** ps,char* es);
void parsepipe(char** ps,char* es);

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
struct backcmd{
    int type;
    struct cmd* cmd;
}

struct cmd* makeback(struct cmd* subcmd)
{
    struct backcmd* backcmd;
    backcmd = (struct backcmd*)malloc(sizeof(struct backcmd));
    memset(backcmd,0,sizeof(*backcmd));

    backcmd->type = BACK;
    backcmd->cmd=subcmd;
    return (struct cmd*)backcmd;
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
    char *ps,*es;
    ps = buf;
    *es=ps+strlen(buf);
    parsekline(&ps,es);
}
void parseline(char**ps,char*es)
{
    char*p = *ps;
    struct cmd* cmd;

    peek(p,es,""); // skip the whitespace
    
    cmd = parsepipe(ps,es);
    if (peek(ps,es,"&"))
    {
         cmd = makeback(cmd);
    }
    if ()





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
