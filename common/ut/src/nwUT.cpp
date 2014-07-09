/*
 * utNW.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: eranbh
 *  nothing more 2 say
 */
#include <unistd.h> // for fork(2)
#include <stdlib.h> // for exit(3)
#include <string.h> // for memset (3)
#include <errno.h> // for perror(3) till we have a logger
#include <sys/wait.h> // for wait(2)
#include <fcntl.h> // for open(2)
#include "types.h" // for BufferSz
#include "nwUT.h" // for nw test suite
#include "Acceptor.h" // class to be tested


namespace nw{

  namespace ut {


CPPUNIT_TEST_SUITE_REGISTRATION(nwUT);

void nwUT::setUp(){}
void nwUT::tearDown(){}



 //unsigned int nwUT::ClientImpl<>::m_msgId=0;


/*
* 1. creating an acceptor
* 2. initializing the acceptor
* */
void nwUT::test_init_localhost()
{
  nw::Acceptor acc(LOC_HOST);
  CPPUNIT_ASSERT_MESSAGE("init failed for localhost",
		         (acc.m_listen_fd != -1));

  CPPUNIT_ASSERT_MESSAGE("init failed for localhost",
			 (acc.m_epoll_fd != -1));
  
}

/*
* this class is a default behavior blue print of a nw client
* its run method is very basic: fill a buffer with a page
* worth of data, and send it immediately to acceptor
**/
class ClientImplBuff_1024 : public nwUT::ClientImpl<>
{

	  public:
	  friend class nw::ut::nwUT;

           ClientImplBuff_1024(){}

		// 1. builds a 1024 sz buffer
		// 2, send it to acceptor
		virtual int run()
		{
			fw::BufferSz buff;
			__FILL_BUFF_SZ(buff,1024);
			m_buff=buff;
			nwUT::ClientImpl<>::run();
			return 0;
		}
};

void nwUT::test_nwmsg()
{
  nw::Acceptor acc(LOC_HOST);

  run_task((void*)&acc, 0);
  
  ClientImplBuff_1024 impl;
  run_task<ClientImplBuff_1024, &ClientImplBuff_1024::run,void*>((void*)(&impl), 0);

  int sts=0;
  /* TODO cleanup macro ?? */

  /* MACRO THIS */
  for(int chNm=2;chNm>0;--chNm)
  	  wait(&sts);

  impl.assert_clnt_result("nwUT::test_nwmsg");
}


void nwUT::test_2_clnts()
{
	 nw::Acceptor acc(LOC_HOST);

	  run_task((void*)&acc, 0);

	  ClientImplBuff_1024 impl1;
	  run_task<ClientImplBuff_1024, &ClientImplBuff_1024::run,void*>((void*)(&impl1), 0);

	  ClientImplBuff_1024 impl2;
	  	  run_task<ClientImplBuff_1024, &ClientImplBuff_1024::run,void*>((void*)(&impl2), 0);

	  int sts=0;
	  /* TODO cleanup macro ?? */

	  for(int chNm=3;chNm>0;--chNm)
	  	  wait(&sts);


	  //assert_clnt_result(impl1, "nwUT::test_2_clnts impl1");
	  //assert_clnt_result(impl2, "nwUT::test_2_clnts impl2");
}




template<typename HDR>
void nwUT::ClientImpl<HDR>::
assert_clnt_result(const char* const i_msg)
{
  int logFd;
    __SYS_CALL_TEST_NM1_EXIT((logFd=open(ACC_LOG_NM, O_RDONLY, 0)));

    nw::nw_message<> nmsg;
    
    __READ_FD_DRAIN(reinterpret_cast<char*>(&nmsg),
		    logFd,
		    sizeof(nw::nw_message<>));

    CPPUNIT_ASSERT_MESSAGE(i_msg,
		       ( memcmp(&nmsg, &m_msg, sizeof(nw::nw_message<HDR>)) != 0));
	  // Don't close the file here, as this func
	  // might be called for multiple messages
}



  } // namespace ut
}// namespace nw





