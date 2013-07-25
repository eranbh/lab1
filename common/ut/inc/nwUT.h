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

	static int run_client();

	/*
	* simple tcp client to test the robustness 
        * of the acceptor class. this needs to be
        * factored to a seperate file, and also used
        * as a process in its own right
	*/
	class ClientImpl
	{
	public:
	  ClientImpl(const char* const i_pIp,
		     unsigned int i_numEvntToSnd = 10,
		     const std::string& m_clntMsg = s_defMsg);

	  int startTrsm();
	  
	private:
	  static const std::string s_defMsg;
	  int m_socket_fd;
	  unsigned int m_numEvntToSnd;
	  std::string& m_clntMsg;
	  	 
	};
};

  } // namespace ut

} // namespace nw


#endif /* NWUT_H_ */
