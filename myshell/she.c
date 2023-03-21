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
#define LIST 5

char whitespace[]="\t\r\n\v";
char symbols[] = "<|>&;()";

void perr(char *s)
{
    printf("%s:something wrong",s);
}

void peek(char**ps,char*es,char*toks)
{
    char*s=*ps;
    while (s<es && strchr(whitespace,*s))
        s++;
    *ps=s;
    return *s && strchr(toks,*s); 
}

char gettoken(char**ps,char*es,char**content)
{
    char* p= *ps;
    char* s;
    char ret;
    peek(p,es,"");
    
    ret=*p;
    switch(*p)
    {
        
        case 0:break;
        /*
        case '|':
        case '&':
        case '<':
        case ';':
        case '(':

        case ')':
               p++;
               break;
               */
        case '>':
               p++;
               if (*p=='>')
               {
                   ret='+';
                   p++;
               }
               else ret='n';
               break;
        defalt:
               ret='a';
               p++;

        
    }
    peek(p,es,"");
    if (!content) return ret;
    s=p;
    while(p<es && !strchr(symbols,*p))
        p++;
    len=strlen(s)-strlen(p);   //98s654p21'0' s-p:7-3=4

    char* cont = (char*)malloc(sizeof(char)*(len+1));
    snprintf(cont,len+1,"%s",s);
    *content=cont;

    return ret;
}


void runcmd(struct cmd* cmd);
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
struct cmd* makelist(struct cmd* subcmd1,struct cmd* subcmd2)
{
    struct listcmd* listcmd;
    listcmd = (struct listcmd*)malloc(sizeof(struct listcmd));
    memset(listcmd,0,sizeof(*listcmd));

    listcmd->type = LIST;
    listcmd->left = subcmd1;
    listcmd->right = subcmd2;

    return (struct cmd*)listcmd;
}
struct cmd* makepipe(struct cmd* subcmd1,struct cmd* subcmd2)
{
    struct pipecmd* pipecmd;
    pipecmd = (struct pipecmd*)malloc(sizeof(struct pipecmd));
    memset(pipecmd,0,sizeof((*pipecmd)));

    pipecmd->type = PIPE;
    pipecmd->left = subcmd1;
    pipecmd->right = subcmd2;

    return (struct cmd*)pipecmd;
}
struct cmd* makeredir(struct cmd* subcmd)
{
    struct redircmd* redircmd;
    redircmd = (struct redircmd*)malloc(sizeof(struct redircmd));

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

void runcmd(struct cmd* cmd)
{
    struct listcmd* listcmd;
    struct backcmd* backcmd;
    switch(cmd->type){
        case LIST:
            listcmd=(struct listcmd*)cmd;
            if (fork()==0)
                runcmd(listcmd->left);
            wait();
            runcmd(listcmd->right);
            break;
        case BACK:
            backcmd=(struct backcmd*)cmd;
            if (fork()==0)
                runcmd(backcmd->cmd);
            break;

    }
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
    parseline(&ps,es);
}
struct cmd* parseline(char**ps,char*es)
{
    char*p = *ps;
    struct cmd* cmd;

    peek(&p,es,""); // skip the whitespace
    
    cmd = parsepipe(ps,es);
    if (peek(&p,es,"&"))
    {
        cmd = makeback(cmd);
        p++;
        peek(&p,es,"");
    }
    if (peek(&p,es,";"))
    {
        cmd = makelist(cmd,parseline(ps,es));
        p++;
        peek(&p,es,"");
    }

    return cmd;
}
void parsepipe(char**ps,char*es)
{
    char* p=*ps;
    struct cmd* cmd;

    cmd=parseredir(&p,es);
    if (peek(&p,es,"|"))
    {  
        cmd = makepipe(cmd,parsepipe(&p,es));
        p++;
    }
    peek(&p,es,"");
}
void parseredir(char**ps,char*es)
{
    char*p=*ps;
    struct cmd* cmd;

    cmd=parseexec(&p,es);
    if (peek(&p,es,"><"))
    {
        char sym;
        char filename[200];
        
        sym = gettoken(&ps,es,filename);


        cmd = makeredir(oldfd,newfile,cmd);
    }
}
