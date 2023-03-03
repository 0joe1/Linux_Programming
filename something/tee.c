#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>  //getopt()

#define BUFSIZE 1024

int main(int argc,char *argv[])
{
	int fd,opt;
	char buf[BUFSIZE];

	int choice=0;
	char* file = (char*)malloc(sizeof(char)*20);
	if ((opt = getopt(argc,argv,"a:")) != -1)
	{
		switch(opt)
		{
			case 'a': choice=1;
					  break;
			defalt:printf("参数错误");
				   _exit(0);

		}
	}

	int openflags;
	if (choice==1)
	{
		file = optarg;
		openflags=O_WRONLY|O_CREAT|O_APPEND;
	}
	else
	{
		file = argv[1];
		openflags=O_WRONLY|O_CREAT|O_TRUNC;
	}
	fd=open(file,openflags,S_IRUSR|S_IWUSR);

	while (read(STDIN_FILENO,&buf,1) > 0)
	{
		printf("%c",*buf);
		write(fd,&buf,1);
	}
	close(fd);
}

