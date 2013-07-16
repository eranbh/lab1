/*
 * nwUT.h
 *
 *  Created on: Jun 19, 2013
 *      Author: eranbh
 */

#ifndef NWUT_H_
#define NWUT_H_

#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"


namespace nw {

  namespace ut {

class nwUT : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(nwUT);

	CPPUNIT_TEST(test_init_localhost);
	CPPUNIT_TEST(test_nwmsg);

	CPPUNIT_TEST_SUITE_END();


 public:
	nwUT(){}
	virtual ~nwUT(){}
	virtual void setUp();
	virtual void tearDown();
	
	void test_init_localhost();
	void test_nwmsg();

 private:
	static int run_srv();
};

  } // namespace ut

} // namespace nw


#endif /* NWUT_H_ */
