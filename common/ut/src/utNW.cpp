/*
 * utNW.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: eranbh
 */
#include "nwUT.h"

namespace nw{

  namespace ut {

CPPUNIT_TEST_SUITE_REGISTRATION(utNW);

void nwUT::setUp(){}
void nwUT::tearDown(){}


/*
* 1. creating an acceptor
* 2. initializing the acceptor
* */
void nwUT::test_init_localhost()
{
  nw::acceptor acc("localhost");
  CPPUNIT_ASSERT_NOT_EQUAL_MESSAGE("init failed for localhost",
			       acc.m_listen_fd,
			       -1);

  CPPUNIT_ASSERT_NOT_EQUAL_MESSAGE("init failed for localhost",
				   acc.m_epoll_fd,
				   -1);
  
}

  } // namespace ut
}// namespace nw





