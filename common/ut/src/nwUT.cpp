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
#include <netinet/in.h> // for struct sockaddr_in
#include <netdb.h> // for gethostbyname(3)
#include <sys/wait.h> // for wait(2)
#include <fcntl.h> // for open(2)
#include "nwUT.h" // for nw test suite
#include "Acceptor.h" // class to be tested


namespace nw{

  namespace ut {

CPPUNIT_TEST_SUITE_REGISTRATION(nwUT);

void nwUT::setUp(){}
/*this is global so I can close it on tear-down */
int g_logFd=0;
void nwUT::tearDown(){__SYS_CALL_TEST_NM1_EXIT(close(g_logFd));}


static const std::string LOC_HOST("localhost");
static const unsigned short PORT=8002;

unsigned int nwUT::ClientImpl::m_msgId=0;


/*
* 1. creating an acceptor
* 2. initializing the acceptor
* */
void nwUT::test_init_localhost()
{
  nw::Acceptor acc(LOC_HOST.c_str());
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
class ClientImplBuff_1024 : public nwUT::ClientImpl
{

	  public:
	  friend class nw::ut::nwUT;

		ClientImplBuff_1024(nw_message::tMsgTypes i_msgTyp = nw_message::TRM):
			                                              ClientImpl(i_msgTyp){}

		// 1. builds a 1024 sz buffer
		// 2, send it to acceptor
		virtual int run()
		{
			fw::BufferSz buff;
			__FILL_BUFF_SZ(buff,1024);
			m_buff=buff;
			nwUT::ClientImpl::run();
			return 0;
		}
};

void nwUT::test_nwmsg()
{
  nw::Acceptor acc(LOC_HOST.c_str());

  run_task((void*)&acc, 0);
  
  ClientImplBuff_1024 impl;
  run_task<ClientImplBuff_1024, &ClientImplBuff_1024::run,void*>((void*)(&impl), 0);

  int sts=0;
  /* TODO cleanup macro ?? */

  /* MACRO THIS */
  for(int chNm=2;chNm>0;--chNm)
  	  wait(&sts);

  assert_clnt_result(impl, "nwUT::test_nwmsg");
}


void nwUT::test_2_clnts()
{
	 nw::Acceptor acc(LOC_HOST.c_str());

	  run_task((void*)&acc, 0);

	  ClientImplBuff_1024 impl1(nw_message::REG);
	  run_task<ClientImplBuff_1024, &ClientImplBuff_1024::run,void*>((void*)(&impl1), 0);

	  ClientImplBuff_1024 impl2;
	  	  run_task<ClientImplBuff_1024, &ClientImplBuff_1024::run,void*>((void*)(&impl2), 0);

	  int sts=0;
	  /* TODO cleanup macro ?? */

	  for(int chNm=3;chNm>0;--chNm)
	  	  wait(&sts);


	  assert_clnt_result(impl1, "nwUT::test_2_clnts impl1");
	  assert_clnt_result(impl2, "nwUT::test_2_clnts impl2");
}



/* make the clever compilers happy */
static fw::BufferSz dummyBfSz;


/* Generic nw client impl code */

nwUT::ClientImpl::
ClientImpl(nw_message::tMsgTypes i_msgTyp,
		   const char* const i_pIp,
           unsigned int i_numEvntToSnd):
            					m_buff(dummyBfSz),
            					m_numEvntToSnd(i_numEvntToSnd)
{
   struct sockaddr_in server_info;
   struct hostent *he;

  if ((m_socket_fd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
     fprintf(stderr, "Socket Failure!!\n");
     exit(1);
  }

   if ((he = gethostbyname(i_pIp))==NULL) {
      fprintf(stderr, "Cannot get host name\n");
      exit(1);
  }

  memset(&server_info, 0, sizeof(server_info));
  server_info.sin_family = AF_INET;
  server_info.sin_port = htons(PORT);
  server_info.sin_addr = *((struct in_addr *)he->h_addr);
  if (connect(m_socket_fd,
	      (struct sockaddr *)&server_info, 
	      sizeof(struct sockaddr))<0) 
  {
    //fprintf(stderr, "Connection Failure\n");
    perror("connect");
    exit(1);
  }

  /* looks like we are all set. */
  m_msg.set_msgTyp(i_msgTyp); /* one time msg */
}


void
nwUT::assert_clnt_result(ClientImpl& i_clnt,
					     const char* const i_msg)
{
	 __SYS_CALL_TEST_NM1_EXIT((g_logFd=open(ACC_LOG_NM, O_RDONLY, 0)));

	  nw::nw_message nmsg;

	  __READ_FD_DRAIN(reinterpret_cast<char*>(&nmsg),
			          g_logFd,
	                 sizeof(nw::nw_message));

	  CPPUNIT_ASSERT_MESSAGE(i_msg,
	  		         ( memcmp(&nmsg, &i_clnt.m_msg, sizeof(nw::nw_message)) != 0));
	  // Don't close the file here, as this func
	  // might be called for multiple messages
}

  } // namespace ut
}// namespace nw





