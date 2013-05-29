#include <stdio.h> // for putchar(3)
#include <sys/types.h>  // types for socket(2)
#include <sys/socket.h> // for socket(2)
#include <netinet/in.h> // for sockaddr_in
#include <sys/epoll.h> // for epoll(7)
#include <assert.h> // for asser (3)
#include <string.h> // for memset(3)
#include <errno.h> // for errno (3)
#include <unistd.h> // for read(2)
#include <stdlib.h> // for exit(3)
#include "macros.h" // my macros
#include "acceptor.h"
#include "nw_message.h" // network message stuff


namespace mem_db {
  namespace nw {

int
acceptor::init(const char* const ip_ip)
{
  assert(0 < ip_ip);

  sockaddr_in addr_in;
  
  
  __SYS_CALL_TEST_NM1_RETURN((m_listen_fd=socket(AF_INET, SOCK_STREAM, 0)));
  
  memset(&addr_in, 0, sizeof(sockaddr_in));

  addr_in.sin_family = AF_INET;
  addr_in.sin_port = 9898;
  addr_in.sin_addr.s_addr = INADDR_ANY;

  
  __SYS_CALL_TEST_NM1_RETURN((bind(m_listen_fd,
                                   (struct sockaddr*) &addr_in,
                                   sizeof(addr_in) )));

  __SYS_CALL_TEST_NM1_RETURN(listen(m_listen_fd, 10));

  /* socket is now listenable*/
  __SYS_CALL_TEST_NM1_RETURN( (m_epoll_fd=epoll_create(10/*not used*/)));
    
  return 0;
}

// this might need to go in consts header 
const unsigned short MAX_EVENTS=10;

/*
* This might block in that - oh so rare case - where
* there are no clients trying to xmit their stuff over
* This assumes that setting up is over, and listens forever
*/
int
acceptor::listen_2_events()
{
  /* We want an edge trigered reactor. This means its our responsability to
     to consume the entire buffer every time an event occurs. otherwise we hang
     Max number of events should have some corolation to the amount of threads
     timeout on the wait needs to be specified
     if there are other tasks [set forever] When accepting, some errors are 
     not fatal. Need to screen for those*/

  epoll_event event, events[MAX_EVENTS];
  event.events=EPOLLIN | EPOLLRDHUP | EPOLLET;
  event.data.fd = m_listen_fd;

  /* first setup should always succeed. failure is not an option*/
  __SYS_CALL_TEST_NM1_RETURN(epoll_ctl(m_epoll_fd, 
                                       EPOLL_CTL_ADD, 
                                       m_listen_fd, 
                                       &event));

  int fds=0;

  /* may you live forever*/
  while(1)
  {
    // TODO num of events, timeout
    if(0 > (fds=epoll_wait(m_epoll_fd, &event, 10, -1)))
    {
      /* this is the only case I could think of that we might be interested*/
      if(EINTR != errno) 
	return -2;
      printf("interrupt happened - maybe we care?");      
    }

    /* knock knock ... who's there?*/

    for(int i=0;i<fds;++i)
    {
      /* new connection*/
      if (events[i].data.fd == m_listen_fd) 
      {
	int acc_fd;
	sockaddr_in addr_in;
	socklen_t addrlen=sizeof(sockaddr_in);
	
	/* TODO accept remarks above*/
	__SYS_CALL_TEST_NM1_RETURN((acc_fd=accept(m_listen_fd,
				                  (struct sockaddr *) &addr_in, 
		 				   &addrlen)));
	// not deciding if to block here. the worker will do that
	event.events = EPOLLIN|EPOLLET;
	event.data.fd = acc_fd;
	// pass it to a thread here. its epoll will do completion
	__SYS_CALL_TEST_NM1_RETURN( epoll_ctl(m_epoll_fd, 
                                              EPOLL_CTL_ADD,
                                              event.data.fd,
					      &event) );
      }
      else /* this is only here till we setup pthreads*/
	handle_request(events[i].data.fd);
    }
  }
}

/*
* Handle request
*/
int
acceptor::handle_request(int fd)
{
  printf("request on fd %d - draining\n", fd);

  nw_message nmsg;

  // drain header
  __READ_FD_DRAIN(reinterpret_cast<char*>(&nmsg.m_header),
                  fd,
                  sizeof(nw_message::header));

  /* The decision whether to drain the body has to be made while taking other
     tasks that might need to be executed, in consideration*/

  __READ_FD_DRAIN(nmsg.m_body, 
                  fd, 
		  nmsg.m_header.m_msg_sz);

  // Testing only!!!
  printf("handling request of type [%u].\n", nmsg.m_header.m_msg_type);
  printf("msg body:\n");
  for(uint32 i=0;i<nmsg.m_header.m_msg_sz;++i)
    putchar(nmsg.m_body[i]);
  return 0;
}

  } // namespace nw
} // namespace mem_db
