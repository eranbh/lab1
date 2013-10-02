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
#include "Acceptor.h" // the class in question
#include


namespace nw {

  namespace ut {

class nwUT : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(nwUT);

	CPPUNIT_TEST(test_init_localhost);
	CPPUNIT_TEST(test_nwmsg);
	CPPUNIT_TEST(test_2_clnts);

	CPPUNIT_TEST_SUITE_END();


 public:
	nwUT(){}
	virtual ~nwUT(){}
	virtual void setUp();
	virtual void tearDown();
	
	void test_init_localhost();
	void test_nwmsg();
	void test_2_clnts();

 private:

	static int run_client();
	static int run_srv();

	/*
	* simple tcp client to test the robustness 
        * of the acceptor class. this needs to be
        * factored to a seperate file, and also used
        * as a process in its own right
	*/
	class ClientImpl
	{
	public:
	  /* we need the friendhip so we can access private members of kids*/
	  friend class nw::ut::nwUT;
	  ClientImpl(const char* const i_pIp,
		         unsigned int i_numEvntToSnd = 1,
		         const char* const m_clntMsg = s_defMsg.c_str());

	  /* default impl */
	  virtual int run()
	  {
		  char buff[1024];
		  __FILL_ARRAY_BYTE_SZ(buff,1024);
		  m_msg.init(buff, 1024, nw_message::TRM);

		  // client is suppose to be constructed by now
		  // write the entire content of the buff to the socket
		  __WRITE_FD_DRAIN(&m_msg, m_socket_fd, sizeof(nw::nw_message));

		  close(m_socket_fd);
		  return 0;
	  }
	  
	  virtual void init(void* ) {} /*only to allow an init with void arg in case its not needed */

	private:
	  static const std::string s_defMsg;
	  int                      m_socket_fd;
	  unsigned int             m_numEvntToSnd;
	  nw::nw_message           m_msg;
	  	 
	};

	template <typename T = nw::Acceptor,
			  int (T::* pFunc) () = &nw::Acceptor::listen_2_events,
			  typename CAST_ARGS = char* >
    static int run_task(void* pobj, void* pargs)
	{
	    pid_t pid=0;

	    if(0 > (pid=fork())) // TODO indicate the failure somehow
	      return -1;

	    /* go ahead son. show me what you've got */
	    if(0 == pid)
	    {
	      T* pimpl = static_cast<T*>(pobj);
	      (pimpl->*pFunc)();
	      exit(0);
	    }
	    
	    return pid;
	}
};

  } // namespace ut

} // namespace nw


#endif /* NWUT_H_ */
