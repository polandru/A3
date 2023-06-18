#include "systemcalls.h"
#include "unistd.h"
#include "errno.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "fcntl.h"
#include "string.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
 */
bool do_system(const char *cmd)
{

    /*
     * TODO  add your code here
     *  Call the system() function with the command set in the cmd
     *   and return a boolean true if the system() call completed with success
     *   or false() if it returned a failure
     */
    system(cmd);
    return errno == 0;
}

/**
 * @param count -The numbers of variables passed to the function. The variables are command to execute.
 *   followed by arguments to pass to the command
 *   Since exec() does not perform path expansion, the command to execute needs
 *   to be an absolute path.
 * @param ... - A list of 1 or more arguments after the @param count argument.
 *   The first is always the full path to the command to execute with execv()
 *   The remaining arguments are a list of arguments to pass to the command in execv()
 * @return true if the command @param ... with arguments @param arguments were executed successfully
 *   using the execv() call, false if an error occurred, either in invocation of the
 *   fork, waitpid, or execv() command, or if a non-zero return value was returned
 *   by the command issued in @param arguments with the specified arguments.
 */

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed

    /*
     * TODO:
     *   Execute a system command by calling fork, execv(),
     *   and wait instead of system (see LSP page 161).
     *   Use the command[0] as the full path to the command to execute
     *   (first argument to execv), and use the remaining arguments
     *   as second argument to the execv() command.
     *
     *
     */
    if (command[0][0] != '/')
    {
        return false;
    }
    

    int pid = fork();
    if (!pid)
    {
        execv(command[0], command);
    }
    else if (pid > 0)
    {
        int status;
        pid_t rt;
        rt = wait(&status);
        if (rt == -1)
        {
            perror("Wait error");
        }
        if(WEXITSTATUS(status) != 0 ){
            return false;
        }
    }
    else if (pid == -1)
    {
        perror("Fork error");
    }

    va_end(args);

    return true;
}

/**
 * @param outputfile - The full path to the file to write with command output.
 *   This file will be closed at completion of the function call.
 * All other parameters, see do_exec above
 */
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    /*
     * TODO
     *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
     *   redirect standard out to a file specified by outputfile.
     *   The rest of the behaviour is same as do_exec()
     *
     */
    // Command validation
    if (command[0][0] != '/')
    {
        return false;
    }


    int kidpid;
    int fd = open(outputfile, O_WRONLY| O_CREAT, 0666);
    if (fd < 0)
    {
        perror("open");
        abort();
    }
    switch (kidpid = fork())
    {
    case -1:
        perror("fork");
        abort();
    case 0:
        if (dup2(fd, 1) < 0)
        {
            perror("dup2");
            abort();
        }
        close(fd);
        execv(command[0], command);
        perror("execv");
        abort();
    default:
        close(fd);
        int status;
        wait(&status);
        /* do whatever the parent wants to do. */
    }
    va_end(args);

    return true;
}