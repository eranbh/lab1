/*
 * utNW.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: eranbh
 */
#include "nwUT.h"

namespace nw {

CPPUNIT_TEST_SUITE_REGISTRATION(utNW);

void nwUT::setUp(){}
void nwUT::tearDown(){}


/*
* 1. creating an acceptor
* 2. initializing the acceptor
* */
void nwUT::test_init_localhost()
{
	acceptor acc;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("init failed for localhost",
			                     acc.init("localhost"),
			                     0);
}

} // namespace nw




