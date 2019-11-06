/**
 * in our world we often use forking to spawn our work.
 * this is usually fine, as long as certain rules are adhered to.
 * there is, however a certain case, in which using fork might
 * lead to issues: MT processes. <add reference here>
 * this poc attempts to achieve 2 goals:
 * 1. avoid UB at all coasts by using a [member] of the exec
 *    family after a fork.
 * 2. be effeciant in the sense that no big copy operations will be
 *    done during this process.
 *
 * the big idea: 
 * 1. using memfd_create(2) to obtain an "exec'able" fd
 * 2. using mmap(2) to provide actual memory to execute
 *
 * known limitations:
 * 1. memfd_create was introduced in the 3.17 kernel, which puts
 *    a lower bound of Android API level 23 [6.0 Marshmallow] on the user.
 */

#include <iostream> // for all out printing 
#include <fcntl.h> // for open(2) related enums
#include <unistd.h> // for read(2) / write(2)

#ifdef SHELL_CODE
// this creates the program that we want to exec
int main()
{
    std::cout << "running our shell code here. nothing to see" << std::endl;
    // secretly planting a file on disk somewhere ... hee hee
    int fd = -1;
    if((fd = open("aaa", O_WRONLY | O_APPEND | O_CREAT, 0644))) return 7;
    write(fd, "bla", 3);
    return 77;
}
#endif // SHELL_CODE


#ifdef LOADER_CODE
#include <sys/syscall.h> // for syscall(2)
#include <linux/memfd.h> // memfd_create(2)
#include <stdlib.h> // exit(3)
#include <sys/wait.h> // for waitpid(2)

// we assume the code above is compiled when this gets exec'ed
// this is the size of the binary known at compile time
const int SHELL_CODE_SIZE = 8240;
const char* SHELL_CODE_FILE_NAME = "shell_code_file";

int main()
{
    // create an fd for the to-be-exec'ed file
    int fd = -1;
    //if ((fd = memfd_create("shell_code_file", MFD_CLOEXEC)) < 0) exit (1);
    if ((fd = syscall(SYS_memfd_create, SHELL_CODE_FILE_NAME, MFD_CLOEXEC)) < 0) return (1);

    // this simulates the buffer we will get from server 
    // containing the shell code.
    // just imagine this is received through a socket
    char shell_code_buff[SHELL_CODE_SIZE] = {0};
    int shell_code_fd = -1;
    if(-1 == (shell_code_fd = open(SHELL_CODE_FILE_NAME, O_RDONLY))) return(2);
    if(-1 == read(shell_code_fd, shell_code_buff, SHELL_CODE_SIZE)) return(3);
    if(-1 == write(fd, shell_code_buff, SHELL_CODE_SIZE)) return(4);

    // now exec the program. hey, exec with no file on disc!!!!!
    // the proper way to do it is read from proc [/proc/self/fd/NNN]
    // and then stat the file. here im taking a shortcut
    // we do it in a seperate process ... naturally
    pid_t pid = -1;
    if(0 > (pid=fork())) return(5);
    if(0 == pid){
        // child
        char* args[2]= {(char*)"shell_code_file", NULL};
        if(-1 == fexecve(fd, args, environ)) return(6);
    }


    int status = -1;
    waitpid(pid, &status, 0);

    
    return 88;
}
#endif // LOADER_CODE
