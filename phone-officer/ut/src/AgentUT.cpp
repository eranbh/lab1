/*
 * AgentUT.cpp
 *
 */
#include <fcntl.h> // for open(2) related stuff
#include <unistd.h> // for execv(3), for fork(2)
#include <sys/wait.h> // for wait(2)
#include "macros.h"
#include "AgentUT.h" // for nw test suite


namespace po{

  namespace ut {


CPPUNIT_TEST_SUITE_REGISTRATION(AgentUT);

void AgentUT::setUp(){}
void AgentUT::tearDown(){}

// TODO use this in the future
//static const char* PROJ_ROOT="/home/eran/work/lab/phone-officer/ut/preloaded";

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

   int pid=0;
   __SYS_CALL_TEST_NM1_EXIT(pid=fork());

   if(0 == pid)
   {
       // exec the cmd
       char* argss[] = {"open", 0};
       __SYS_CALL_TEST_NM1_EXIT(execv("/home/eran/work/lab/phone-officer/ut/preloaded/open", argss));
   }
   // making sure the process finished
   int sts=0;
   __SYS_CALL_TEST_NM1_EXIT(wait(&sts));
  // assert the sts    
}


  } // namespace ut
}// namespace po


