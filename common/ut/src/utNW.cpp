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
#include "nwUT.h" // for nw test suite
#include "acceptor.h" // class to be tested

namespace nw{

  namespace ut {

CPPUNIT_TEST_SUITE_REGISTRATION(nwUT);

void nwUT::setUp(){}
void nwUT::tearDown(){}


static const std::string LOC_HOST("localhost");
static const unsigned short PORT=3490;
static const unsigned short MAXSIZE=1024;

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
  
  
}


/*
* this func forks a new thread for the client code.
* this would be easily replaced by either boost/ace tasks
*/
int nwUT::run_client()
{
  pid_t pid=0;

  if(0 > (pid=fork())) // TODO indicate the failure somehow
    return -1;

  /* go ahead son. show me what you've got */
  if(0 == pid)
  {
    
  }

  return 0;
}



nwUT::ClientImpl::
ClientImpl(const char* const i_pIp,
           unsigned int i_numEvntToSnd = 10,
	   const std::string& i_msg,
	   std::string& i_clntMsg):m_numEvntToSnd(i_numEvntToSnd),
				   m_clntMsg(i_clntMsg)
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

int 
nwUT::ClientImpl::startTrsm()
{
  char buffer[1024];
 
  //buffer = "Hello World!! Lets have fun\n";
  //memset(buffer, 0 , sizeof(buffer));
  while(1) 
  {
    //fgets(buffer,MAXSIZE-1,stdin);
    if ((send(m_socket_fd,m_clntMsg.c_str(), m_clntMsg.size(),0))== -1) {
      fprintf(stderr, "Failure Sending Message\n");
      close(m_socket_fd);
      exit(1);
    }
    else {
      printf("Message being sent: %s\n",buffer);
    }
  }   

   /*if ((num = recv(socket_fd, buff, 1024,0))== -1) {
   //fprintf(stderr,"Error in receiving message!!\n");
   perror("recv");
   exit(1);
   }   
   //  num = recv(client_fd, buffer, sizeof(buffer),0);
   buff[num] = '\0';
   printf("Message received: %s\nNumber of bytes received: %d\n", buff,num);*/
 close(m_socket_fd); 
  return 0;
}



  } // namespace ut
}// namespace nw





