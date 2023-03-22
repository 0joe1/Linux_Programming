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
                   ret='+';
                   p++;
               }
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
    char* newfile;
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
struct cmd* makeredir(int oldfd,char* newfile,int mode,struct cmd* subcmd)
{
    struct redircmd* redircmd;
    redircmd = (struct redircmd*)malloc(sizeof(struct redircmd));
    memset(redircmd,0,sizeof(*redircmd));

    redircmd->type = REDIR;
    redircmd->oldfd=oldfd;
    redircmd->newfile=newfile;
    redircmd->mode=mode;
    redircmd->cmd=subcmd;

    return (struct cmd*)redircmd;
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
    int p[2];
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
        case PIPE:
            pipecmd=(struct pipecmd*)cmd;
            pipe(p);
            if (fork()==0)
            {
                dup2(1,p[1]);
                runcmd(pipecmd->left);
            }
            if (fork()==0)
            {
                dup2(0,p[0]);
                runcmd(pipecmd->right);
            }
            close(p[0]);
            close(p[1]);
            wait();
            wait();
            break;
        case REDIR:
            redircmd=(struct redircmd*)cmd;

            close(redircmd->oldfd);
            open(redircmd->filename,redircmd->mode);
            runcmd(redircmd->cmd);
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
struct cmd* parsepipe(char**ps,char*es)
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

    return cmd;
}
struct cmd* parseredir(char**ps,char*es)
{
    char*p=*ps;
    struct cmd* cmd;

    cmd=parseexec(&p,es);
    while (peek(&p,es,"><"))
    {
        char token;
        char* filename;
        
        token = gettoken(&ps,es,&filename);
        switch(token)
        {
            case '<':
                cmd=makeredir(0,filename,O_RDONLY,cmd);
                break;
            case '>':
                cmd=makeredir(1,filename,O_WRONLY|O_CREAT|O_TRUNC,cmd);
                break;
            case '+':
                cmd=makeredir(1,filename,O_WRONLY|O_CREAT|O_APPEND,cmd);
                break;
        }
    }
    return cmd;
}
struct cmd* parseexec(char**ps,char*es)
{
    char*p=*ps;
    char* filename;
    char* command;

    gettoken(&p,es,command);

    
    
}
