/*
* this module represents the initial code that
* will be executed upon every exec. we are using
* this method in order to instrument, as we want
* be as flexible as possible, and __not__ be a 
* catch-all sort of solution.
* this code will set up everything that is needed
* for a monitored process, and pass it all in env
* variables to the monitored process
*/

/*
* the main exec function to hijec is execve, as that
* is the "backend" for all the other exec family members.
* see man pages: exec(3) and execve(2) for details
*/

int execve(const char *filename,
           char *const argv[],
           char *const envp[])
{
   // first of all, validate the input.
   // in any case there is a bad argument, dont
   // fail the call as there might be special
   // library behaviour for this case.
   // simply forward the call to libc 
}
