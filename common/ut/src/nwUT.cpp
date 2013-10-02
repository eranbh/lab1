/*
 * utNW.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: eranbh
 */
#include <unistd.h> // for fork(2)
#include <stdlib.h> // for exit(3)
#include <string.h> // for memset (3)
#include <errno.h> // for perror(3) till we have a logger
#include <netinet/in.h> // for struct sockaddr_in
#include <netdb.h> // for gethostbyname(3)
#include <sys/wait.h> // for wait(2)
#include "nwUT.h" // for nw test suite
#include "Acceptor.h" // class to be tested
#include "nw_message.h" // for msg structure
#include "macros.h" // for my macros


#define __FILL_ARRAY_BYTE_SZ(ARR,SZ)    \
do{                                     \
  char j='A';                           \
  for(int i=0;i<SZ;++i) ARR[i]=j;	\
}while(0)

namespace nw{

extern
nw_message g_msg;

  namespace ut {

CPPUNIT_TEST_SUITE_REGISTRATION(nwUT);

void nwUT::setUp(){}
void nwUT::tearDown(){}


static const std::string LOC_HOST("localhost");
static const unsigned short PORT=8002;
static const unsigned short MAXSIZE=1024;

const std::string 
nwUT::ClientImpl::s_defMsg="Client Default Msg";


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

void nwUT::test_nwmsg()
{
  nw::Acceptor acc(LOC_HOST.c_str());
  
  class ClientImplBuff_1024 : public ClientImpl
  {

  	  public:
	  friend class nw::ut::nwUT;

		ClientImplBuff_1024(): ClientImpl("localhost"){}

		virtual int run()
		{
			ClientImpl
			return 0;
		}
  };

  run_task((void*)&acc, 0);
  
  ClientImplBuff_1024 impl;
  run_task<ClientImplBuff_1024, &ClientImplBuff_1024::run,void*>((void*)(&impl), 0);

  int sts=0;
  /* TODO cleanup macro ?? */

  for(int chNm=2;chNm>0;--chNm)
  	  wait(&sts);

  /* as the nw msg is static in sz, we can do this safely ... */
  CPPUNIT_ASSERT_MESSAGE("nwUT::test_nwmsg",
  		         ( memcmp(&g_msg, &impl.m_msg, sizeof(nw::nw_message)) != 0));
}


void nwUT::test_2_clnts()
{

}




/* Generic nw client impl code */

nwUT::ClientImpl::
ClientImpl(const char* const i_pIp,
           unsigned int i_numEvntToSnd,
	   const std::string& i_msg):m_numEvntToSnd(i_numEvntToSnd),
				     m_clntMsg(const_cast<std::string&>(i_msg))
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


}

  } // namespace ut
}// namespace nw





