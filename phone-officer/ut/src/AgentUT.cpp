/*
 * AgentUT.cpp
 *
 */
#include <fcntl.h> // for open(2) related stuff
#include <unistd.h> // for fork(2)
#include <sys/wait.h> // for wait(2)
#include "macros.h"
#include "AgentUT.h" // for nw test suite


namespace po{

  namespace ut {


CPPUNIT_TEST_SUITE_REGISTRATION(AgentUT);

void AgentUT::setUp(){}
void AgentUT::tearDown(){}


static const char* const 
AGENT_PRELOAD_LIB_NM = "/home/eran/work/lab/phone-officer/agent/lib/libagent.so"; 

static const char* const 
LD_PRELOAD_ENV_VAR_NM = "LD_PRELOAD";

/*
* 1. place the env variable in env
* 2. test that preloading happened
* */
void AgentUT::test_init_preload()
{
    // add preloading for our module
    __SYS_CALL_TEST_NM1_EXIT(setenv(LD_PRELOAD_ENV_VAR_NM,
                                   AGENT_PRELOAD_LIB_NM,
                                   1));
    pid_t pid = fork();
    if(0 == pid)
    {
        // we are preloaded. try and call a hijacked function
        char* val = getenv("LD_PRELOAD");	
        if(val)
            open("my_file", O_CREAT);
        exit(0);
    }
    int sts = 0;
    wait(&sts); 
}


  } // namespace ut
}// namespace po


