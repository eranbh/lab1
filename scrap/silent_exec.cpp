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


#ifdef ELF_TO_EXEC
// this creates the program that we want to exec
#include <iostream>
int main()
{
    std::cout << "running our shell code here. nothing to see" << std::endl;
    return 0;
}
#endif // ELF_TO_EXEC


#ifdef LOADER_CODE
#include <stdlib.h> // exit(3)
#include <sys/memfd.h> // memfd_create(2)
// we assume the code above is compiled when this gets exec'ed
// this is the size of the binary known at compile time
const int SHELL_CODE_SIZE = 4096;

int main()
{
    // create an fd for the to-be-exec'ed file
    int fd = -1;
    if ((fd = memfd_create("shell_code_file", MFD_CLOEXEC)) < 0) exit (1);

    // map the file compiled to this fd. this simulates mapping done
    // on the buffer we will get from server containing the shell code
    char shell_code_buff[SHELL_CODE_SIZE] = {0};
    if(

    
    return 0;

}
#endif // LOADER_CODE
