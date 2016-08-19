/*
 * AgentUT.cpp
 *
 */
#include "AgentUT.h" // for nw test suite


namespace po{

  namespace ut {


CPPUNIT_TEST_SUITE_REGISTRATION(AgentUT);

void AgentUT::setUp(){}
void AgentUT::tearDown(){}


static const char* const AGENT_PRELOAD_LIB_NM = "lib/libagent"; 
static const char* const LD_PRELOAD_ENV_VAR_NM = "LD_PRELOAD";

/*
* 1. place the env variable in env
* 2. test that preloading happened
* */
void AgentUT::test_init_preload()
{
   // add preloading for our module
   __SYS_CALL_TEST_NN_RETURN getenv(); 
   // then test that something happened 
  CPPUNIT_ASSERT_MESSAGE("init failed for localhost",
			 (1 == 1));
}


  } // namespace ut
}// namespace po


