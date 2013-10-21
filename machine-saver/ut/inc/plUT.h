/*
 * plUT.h
 *
 *  Created on: Oct 11, 2013
 *      Author: eran
 */

#ifndef PLUT_H_
#define PLUT_H_

#include "cppunit/TestFixture.h"

namespace pl{
	namespace ut{

class plUT : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(plUT);

	CPPUNIT_TEST(test_linux);

	CPPUNIT_TEST_SUITE_END();


public:

	plUT(){}
	virtual ~plUT(){}
	virtual void setUp();
	virtual void tearDown();

	void test_linux();
};

	}// namespace ut

} // namespace pl



#endif /* PLUT_H_ */
