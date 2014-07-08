/*
 * nwUT.h
 *
 *  Created on: Jun 19, 2013
 *      Author: eranbh
 */

#ifndef NWUT_H_
#define NWUT_H_

#include <netinet/in.h> // for struct sockaddr_in
#include <netdb.h> // for gethostbyname(3)
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"
#include "Acceptor.h" // the class in question
#include "nw_message.h" // for msg structure
#include "macros.h" // for my macros


#define __FILL_BUFF_SZ(BUFF,SZ)                  \
do{                                              \
  char j='A';                                    \
  for(int i=0;i<SZ;++i) 	                 \
  {                                              \
    BUFF.buff[i]=(j%'Z'+1);                      \
    j++;if('Z'+1==j) j='A'; 		         \
  }                                              \
  BUFF.sz=SZ;                                    \
}while(0)

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


	/*
	* simple tcp client to test the robustness 
        * of the acceptor class. this needs to be
        * factored to a seperate file, and also used
        * as a process in its own right
	*/
	template<typename HDR=nw::header>
	class ClientImpl
	{
	public:
	  /* we need the friendhip so we can access private members of kids*/
	  friend class nw::ut::nwUT;
	  /* Generic nw client impl code */
	
	ClientImpl(const char* const i_pIp="localhost",
		   unsigned int i_numEvntToSnd=1):
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

	}

	  /* default impl */
	virtual int run()
	{
	  HDR& head = m_msg.get_header();
	  m_msg.init(m_buff.buff, m_buff.sz, head.getMsgType());
	  
	  for(unsigned int i=0;i<m_numEvntToSnd;++i)
	    {
			  
	      // client is suppose to be constructed by now
	      // write the entire content of the buff to the socket
	      __WRITE_FD_DRAIN(&m_msg, m_socket_fd, sizeof(nw::nw_message<HDR>));

	    }

	  close(m_socket_fd);
	  return 0;
	}
	  
	virtual void init(void* ) {} /*only to allow an init with void arg in case its not needed */

	  void assert_clnt_result(const char* const);


	protected:

	  /*
	  	* gets the next network message atomically
	  	*/
	  	unsigned int getNxtMsgId()
	  	{
	  		unsigned int newId=0, oldId = 0;
	  		do
	  		{
	  			newId = oldId = m_msgId;
	  			newId += 1; /* there's probably an incr operator ...*/
	  		} while(oldId != __sync_val_compare_and_swap(&m_msgId, oldId, newId));

	  		return newId; /* send back the copy, as here the member can change _again_*/
	  	}


	  fw::BufferSz            m_buff;
	  int                      m_socket_fd;
	  unsigned int             m_numEvntToSnd;
	  nw::nw_message<HDR>      m_msg;
	  static unsigned int      m_msgId;
	};


 protected:
	  const char* const LOC_HOST = "localhost";
	  typedef enum : unsigned short {PORT=8002}tPort;
	  // const unsigned short PORT  = 8002;
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



 private:

	static int run_client();
	static int run_srv();


};

template<typename HDR>
unsigned int nw::ut::nwUT::ClientImpl<HDR>::m_msgId=0;

  } // namespace ut

} // namespace nw


#endif /* NWUT_H_ */
