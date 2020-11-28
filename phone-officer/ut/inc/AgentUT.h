/*
 * AgentUT.h
 *
 */

#ifndef AGENTUT_H_
#define AGENTUT_H_

#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"





namespace po {

  namespace ut {

class AgentUT : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(AgentUT);

	CPPUNIT_TEST(test_init_preload);

	CPPUNIT_TEST_SUITE_END();


 public:
	AgentUT(){}
	virtual ~AgentUT(){}
	virtual void setUp();
	virtual void tearDown();
	
	void test_init_preload();



 private:


};


  } // namespace ut

} // namespace po


#endif /* AGENTUT_H_ */
