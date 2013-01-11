#include <sys/types.h> // for socket
#include <sys/socket.h> // for socket
#include <netinet/in.h> // for the addr structures/funcs
#include <stdio.h> // for perror
#include <string.h> // for memset
#include <unistd.h> // for read
#include <sys/epoll.h> // for ... epoll
#include <netdb.h> // for gethostbyname
#include <stdlib.h> // for exit (3)
#include <fcntl.h>
#include "macros.h"
#include "network_interface.h"


// Build the listen socket 
BlockingTcpAcceptor::
BlockingTcpAcceptor(unsigned short i_port, 
                    unsigned int i_buffSz):
  m_buffSz(i_buffSz)
                   
{

  __SYS_CALL_TEST_EXIT(m_sockFD = socket(AF_INET, SOCK_STREAM, 0))
   int on=1;
  __SYS_CALL_TEST_EXIT(setsockopt( m_sockFD, 
                                     SOL_SOCKET,
                                     SO_REUSEADDR, 
                                     &on, 
                                     sizeof(on) ))
  struct sockaddr_in socAddr;
  unsigned int sz = sizeof(sockaddr);
  ::memset(&socAddr, 0, sz);
  socAddr.sin_family = AF_INET;
  socAddr.sin_port = htons(i_port);
  socAddr.sin_addr.s_addr = INADDR_ANY;
  __SYS_CALL_TEST_EXIT(bind(m_sockFD,
                         (struct sockaddr*)& socAddr,
                         sizeof(socAddr)))

  __SYS_CALL_TEST_EXIT(listen(m_sockFD, SOMAXCONN))
 }


static const unsigned int MAX_EVENTS=1200;
static const unsigned char TRUE=1;
// Thread blocks here when there are no events
int BlockingTcpAcceptor::listen2Events(mem_db::NetworkMsg& o_msg)
{
  //int epollFD=0;
  //__SYS_CALL_TEST_RETURN((epollFD = epoll_create(MAX_EVENTS)))

  ////  printf("after epol creat\n");

  //struct epoll_event ev, events[MAX_EVENTS]; // 3 event each time?
  //ev.events = EPOLLIN;
  //ev.data.fd = m_sockFD;
  //__SYS_CALL_TEST_RETURN(epoll_ctl(epollFD, EPOLL_CTL_ADD, m_sockFD, &ev))
  //  printf("after epol add fd: %d\n",m_sockFD);
    int accFD=0;
  struct sockaddr_in fromAddr;
  socklen_t addrLen;

   // BLOCKING!!! 
   ///int nfds=0;
  ///__SYS_CALL_TEST_RETURN(nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1))

  //printf("after epol wait fd: %d\n",epollFD);

    for( ; ; )
    {
      //if(m_sockFD == events[i].data.fd)
      //{
      //  fcntl(events[i].data.fd, F_SETFL, O_NONBLOCK);
	__SYS_CALL_TEST_RETURN(accFD=accept(m_sockFD,
		                             (sockaddr*)&fromAddr,
			                    &addrLen))
	  //printf("after accpt on fd: %d got fd: %d\n",
	  //events[i].data.fd,accFD);
	  //fcntl(accFD, F_SETFD, O_NONBLOCK ); 
          //ev.events = EPOLLIN | EPOLLET;
          //ev.data.fd = accFD;
          //__SYS_CALL_TEST_RETURN(epoll_ctl(epollFD, EPOLL_CTL_ADD, accFD,&ev))
	  //  printf("after add fd %d to epol\n",accFD);
	  //}
	  //else // handle read data
	  //{
	  //printf("handling fd %d\n", events[i].data.fd);
	  //ComQueryHeader header;
	  return
	 readData(accFD, 
                  sizeof(mem_db::NetworkMsg),
                  reinterpret_cast<char*>(&o_msg));
	//__SYS_CALL_TEST_RETURN(epoll_ctl(epollFD,
	//                                  EPOLL_CTL_DEL, 
	//                                  accFD,
	//                                  0))
         printf("after read data fd %d\n",accFD);
	 //}
	 close(accFD);
    }
  return 0;
}

int BlockingTcpAcceptor::
  readData(int i_fd, unsigned int i_msgSz, char* o_pBuff)
{
  unsigned int amountRd=0;

  printf("Accepted request for %u bytes\n", i_msgSz);
  // Read ENTIRE data
  while( (0 <= (amountRd += read(i_fd, 
                                o_pBuff+amountRd, 
                                i_msgSz-amountRd))) &&
	 (0<i_msgSz-amountRd)  );
  __SYS_CALL_TEST_RETURN(amountRd)
    printf("done with reading: %u bytes\n", amountRd);
 
  return 0;
} 
	
/////////////////////////////////////////

BlockingTcpTransmitor::
BlockingTcpTransmitor(unsigned short    i_port,
                      const char* const i_pHostNm)
{
   struct sockaddr_in server;
    struct hostent * server_ent;

  __SYS_CALL_TEST_EXIT((m_sockFD = socket(AF_INET, SOCK_STREAM, 0)) )

     server_ent = gethostbyname(i_pHostNm);     
    
     bzero(& server, sizeof(server));
     server.sin_family = AF_INET;
     server.sin_addr = * (struct in_addr *)server_ent->h_addr_list[0];
     server.sin_port = htons(i_port);
    
     __SYS_CALL_TEST_EXIT(connect(m_sockFD,
                                    (sockaddr*)&server, 
			            sizeof(server)))
}

int BlockingTcpTransmitor::
writeData(unsigned int i_msgSz, char* i_pBuff)
{
  unsigned int amountWr=0;

  
  // write ENTIRE data
  while( (0 <= (amountWr += write(m_sockFD, 
                                i_pBuff+amountWr, 
                                i_msgSz-amountWr))) &&
	 (0<i_msgSz-amountWr)  );
  __SYS_CALL_TEST_RETURN( amountWr)
    printf("wrote to socket %u bytes\n", amountWr);
    return 0;
}
