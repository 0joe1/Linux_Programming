#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#define CMDSIZE 100
#define MAXARGS 10

#define EXEC 1
#define BACK 2
#define REDIR 3
#define PIPE 4 
#define LIST 5

char whitespace[]=" \t\r\n\v";
char symbols[] = "<|>&;()";

void perr(char *s)
{
    printf("%s:something wrong",s);
}

int peek(char**ps,char*es,char*toks)
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
    peek(&p,es,"");
    
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
        default:
               ret='a';
    }
    peek(&p,es,"");
    if (!content){
        *ps=p;
        return ret;
    }
    
    s=p;
    while(p<es && !strchr(symbols,*p))
        p++;
    int len;
    len=strlen(s)-strlen(p);   //98s654p21'0' s-p:7-3=4

    char* cont = (char*)malloc(sizeof(char)*(len+1));
    snprintf(cont,len+1,"%s",s);
    *content=cont;
    *ps=p;

    return ret;
}

struct cmd{
    int type;
};

void runcmd(struct cmd* cmd);
int getcmd(char* buf);
struct cmd* analyze(char* buf);

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
};
struct cmd*parseline(char** ps,char* es);
struct cmd*parsepipe(char** ps,char* es);
struct cmd*parseexec(char** ps,char* es);
struct cmd*parseredir(char** ps,char* es);

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
struct cmd* makeexec(int argc,char* argv[])
{
    struct execcmd* execcmd;    
    execcmd = (struct execcmd*)malloc(sizeof(struct execcmd));

    execcmd->type = EXEC;
    for (int i=0;i<argc;i++)
    {  
        execcmd->arglt[i]=(char*)malloc(sizeof(char)*200);
        strcpy(execcmd->arglt[i],argv[i]);
    }

    return (struct cmd*)execcmd;
}

char prepath[500];

int main(void)
{
    sigset_t blockmask;

    // block some signal
    sigemptyset(&blockmask);
    sigaddset(&blockmask,SIGINT);
    sigprocmask(SIG_SETMASK,&blockmask,NULL);

    char *buf=(char*)malloc(sizeof(char)*300);
    struct cmd* cmd;

    getcwd(prepath,sizeof(prepath));
    while (getcmd(buf))
    {   
        while (*buf && strchr(whitespace,*buf))
            buf++;
        if (buf[0]=='c' && buf[1]=='d' && buf[2]==' ')
        {
            char path[500];
            switch(*(buf+3))
            {
                case '~':strcpy(path,"/home/username");break;
                case '-':strcpy(path,prepath);break;
                default: strcpy(path,buf+3);
            }
            chdir(path);
            strcpy(path,prepath);
        }
        if (fork()==0)
            runcmd(analyze(buf));
        wait(NULL);
    }
    free(buf);
    return 0;
}

void runcmd(struct cmd* cmd)
{
    int p[2];
    struct execcmd* execcmd;
    struct listcmd* listcmd;
    struct backcmd* backcmd;
    struct pipecmd* pipecmd;
    struct redircmd* redircmd;

    switch(cmd->type){
        case EXEC:
            execcmd=(struct execcmd*)cmd;
            execvp(execcmd->arglt[0],execcmd->arglt);
            break;
        case LIST:
            listcmd=(struct listcmd*)cmd;
            if (fork()==0)
                runcmd(listcmd->left);
            wait(NULL);
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
                dup2(p[1],1);
                close(p[1]);
                close(p[0]);
                runcmd(pipecmd->left);
            }
            if (fork()==0)
            {
                dup2(p[0],0);
                close(p[0]);
                close(p[1]);
                runcmd(pipecmd->right);
            }
            close(p[0]);
            close(p[1]);
            wait(NULL);
            wait(NULL);
            break;
        case REDIR:
            int newfd;
            redircmd=(struct redircmd*)cmd;
            newfd=open(redircmd->newfile,redircmd->mode);
            dup2(redircmd->oldfd,redircmd->newfd);
            if (redircmd->cmd->type == REDIR)
            {
                struct redircmd* nextcmd=(struct redircmd*)redircmd->cmd;
                nextcmd->oldfd=newfd;
            }
            close(newfd);
            runcmd(redircmd->cmd);
            break;
    }
    exit(1);
}

int getcmd(char* buf)
{
    printf("%s","$ ");
    fgets(buf,CMDSIZE,stdin);
    buf[strlen(buf)-1] = '\0';
    if (strcmp(buf,"exit")==0) return 0;
    return 1;
}
struct cmd* analyze(char* buf)
{
    struct cmd* cmd;
    char *ps,*es;
    ps = buf;
    es=ps+strlen(buf);
    cmd=parseline(&ps,es);
    return cmd;
}
struct cmd* parseline(char**ps,char*es)
{
    char*p = *ps;
    struct cmd* cmd;

    peek(&p,es,""); // skip the whitespace
    
    cmd = parsepipe(&p,es);
    if (peek(&p,es,"&"))
    {
        cmd = makeback(cmd);
        p++;
        peek(&p,es,"");
    }
    if (peek(&p,es,";"))
    {
        cmd = makelist(cmd,parseline(&p,es));
        p++;
        peek(&p,es,"");
    }

    *ps=p;
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

    *ps=p;
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
        
        token = gettoken(&p,es,&filename);
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
    *ps=p;
    return cmd;
}
struct cmd* parseexec(char**ps,char*es)
{
    struct cmd* cmd;
    char*p=*ps;
    char* command,*token;

    int argc=0;
    char* argv[MAXARGS];

    gettoken(&p,es,&command);
    token=strtok(command,whitespace);
    while(token != NULL)
    {
        argv[argc++]=token;
        token=strtok(NULL,whitespace);
    }
    cmd = makeexec(argc,argv); 
    *ps=p;

    return cmd;
}
