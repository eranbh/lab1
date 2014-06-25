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
#include <fcntl.h> // for open(2)
#include "macros.h" // my macros
#include "Acceptor.h"
#include "nw_message.h" // network message stuff


namespace nw {

#ifdef __TESTING_MODE
	int g_logFd=0;
#endif // __TESTING_MODE

Acceptor::Acceptor(const char* const p_ip, int i_backlog)
{
  assert(0 < p_ip);

  struct sockaddr_in addr_in;
  
  
  __SYS_CALL_TEST_NM1_EXIT((m_listen_fd=socket(AF_INET, SOCK_STREAM, 0)));
  
  memset(&addr_in, 0, sizeof(struct sockaddr_in));

  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(8002);
  addr_in.sin_addr.s_addr = INADDR_ANY;

  int reuse=1;

  /* lets reuse addresses for better living */
  __SYS_CALL_TEST_NM1_EXIT(setsockopt(m_listen_fd, 
				      SOL_SOCKET, 
				      SO_REUSEADDR,
				      &reuse,
				      sizeof(int)));

  
  __SYS_CALL_TEST_NM1_EXIT((bind(m_listen_fd,
                                   (struct sockaddr*) &addr_in,
				 sizeof(struct sockaddr) )));

  __SYS_CALL_TEST_NM1_EXIT(listen(m_listen_fd, 10));

  /* socket is now listenable*/
  __SYS_CALL_TEST_NM1_EXIT( (m_epoll_fd=epoll_create(10/*not used*/)));


/* i don't want to be bothered with sending the data back
 * to the tester in a sophisticated way. just write to disk */
#ifdef __TESTING_MODE
  __SYS_CALL_TEST_NM1_EXIT((g_logFd=open(ACC_LOG_NM, O_CREAT|O_TRUNC|O_RDWR, S_IRWXU)));
#endif // __TESTING_MODE
}

// this might need to go in consts header 
const unsigned short MAX_EVENTS=10;

/*
* This might block in that - oh so rare case - where
* there are no clients trying to xmit their stuff over
* This assumes that setting up is over, and listens forever
*/
int
Acceptor::listen_2_events()
{
  /* We want an edge trigered reactor. This means its our responsability to
     to consume the entire buffer every time an event occurs. otherwise we hang
     Max number of events should have some corolation to the amount of threads
     timeout on the wait needs to be specified
     if there are other tasks [set forever] When accepting, some errors are 
     not fatal. Need to screen for those*/

  epoll_event event, events[MAX_EVENTS];
  event.events=EPOLLIN;
  event.data.fd = m_listen_fd;

  /* first setup should always succeed. failure is not an option*/
  __SYS_CALL_TEST_NM1_RETURN(epoll_ctl(m_epoll_fd, 
                                       EPOLL_CTL_ADD, 
                                       m_listen_fd, 
                                       &event));

  int fds=0;
  int handleSts=1;

  /* may you live forever*/
  do
  {
	  errno=0;
    // TODO num of events, timeout
    if(0 > (fds=epoll_wait(m_epoll_fd, events, 10, -1)))
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
    	  printf("looking into fd: %d\n", events[i].data.fd);
    	  int acc_fd;
    	  sockaddr_in addr_in;
    	  socklen_t addrlen=sizeof(sockaddr_in);

    	  /* TODO accept remarks above*/
    	  __SYS_CALL_TEST_NM1_RETURN((acc_fd=accept(m_listen_fd,
    			  (struct sockaddr *) &addr_in,
    			  &addrlen)));
    	  // not deciding if to block here. the worker will do that
    	  event.events = EPOLLIN;
    	  event.data.fd = acc_fd;

    	  // pass it to a thread here. its epoll will do completion
    	  __SYS_CALL_TEST_NM1_RETURN( epoll_ctl(m_epoll_fd,
    			  	  	  	  	  	  EPOLL_CTL_ADD,
    			  	  	  	  	  	  event.data.fd,
    			  	  	  	  	  	  &event) );
      }
      else /* this is only here till we setup pthreads*/
      {
    	  printf("event happened\n");
    	  if(events[i].events & EPOLLIN)
    		  handleSts = handle_request(events[i].data.fd);
      }
    }// for
  } while(handleSts);

#ifdef __TESTING_MODE
  __SYS_CALL_TEST_NM1_RETURN(close(g_logFd));
#endif // __TESTING_MODE

  return 0; /* stopped by user */
}

/*
* Handle request
* simple default implementation
*/
int
Acceptor::handle_request(int fd)
{
  printf("request on fd %d - draining\n", fd);

  nw_message<> nmsg;

  // drain header
  __READ_FD_DRAIN(reinterpret_cast<char*>(&nmsg.m_header),
                  fd,
                  sizeof(nw_message<>::header));
  int ret=1;

  switch(nmsg.m_header.m_msg_type)
  {
          case nw_message<>::msg_types::TRM:
  		ret=0;
          case nw_message<>::msg_types::REG:
  	  {
  		/* The decision whether to drain the body has to be made while taking other
  		     tasks that might need to be executed, in consideration*/

  		  __READ_FD_DRAIN(nmsg.m_body,
  		                  fd,
  		  		  nmsg.m_header.m_msg_sz);

  		  // Testing only!!!
#ifdef __TESTING_MODE
  		__WRITE_FD_DRAIN(reinterpret_cast<char*>(&nmsg),
  				  g_logFd,
				  sizeof(nw_message<>));
#else // handle msg
  		printf("handling request of type [%u].\n", nmsg.m_header.m_msg_type);
  		printf("msg body:\n");
  		for(uint32 i=0;i<nmsg.m_header.m_msg_sz;++i)
  			printf("%c",nmsg.m_body[i]);
#endif // __TESTING_MODE
  		break;
  	  }
  	  default: printf("Invalid message type accepted\n");
  		  ret = 0;
  		  break;
  }

  return ret;
}

} // namespace nw

