#include "systemcalls.h"
#include "unistd.h"
#include "errno.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>   
#include "fcntl.h"
#include "string.h"

int main()
{
    char * cmd[3] = {"/bin/echo","home is $HOME", NULL};
    char * env[2] = {"", NULL}; 
    execv("/bin/echo",cmd);
}