#ifndef FILE_PERMS_H
#define FILE_PERMS_H

#include <sys/types.h>
			
#define FP_SPECIAL 1
char *filePermStr(mode_t perm,int flags);
#endif
