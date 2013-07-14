/*
 * utNW.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: eranbh
 */
#include "nwUT.h" // for nw test suite
#include "acceptor.h" // class to be tested

namespace nw{

  namespace ut {

CPPUNIT_TEST_SUITE_REGISTRATION(nwUT);

void nwUT::setUp(){}
void nwUT::tearDown(){}


static const std::string LOC_HOST("localhost"); 

/*
* 1. creating an acceptor
* 2. initializing the acceptor
* */
void nwUT::test_init_localhost()
{
  nw::Acceptor acc(LOC_HOST.c_str());
  CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("init failed for localhost",
			                (acc.m_listen_fd == -1));

  CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("init failed for localhost",
					(acc.m_epoll_fd == -1));
  
}

  } // namespace ut
}// namespace nw





