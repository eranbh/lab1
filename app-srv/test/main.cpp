#include <sys/types.h>  // types for socket(2)
#include <sys/socket.h> // for socket(2)
#include <netinet/in.h> // for sockaddr_in
#include <stdlib.h> // for exit(3)
#include <stdio.h> // for perror(3)
#include <string.h> // for memcpy(3)
#include <unistd.h> // for write(2)
#include <netdb.h> // gethostbyname (3)
#include "macros.h" // my macros

static const char* const HOST="localhost";

/*
* This program simulates an agent.
*/
int
main(int argc, char* argv[])
{
  int out_fd=0, wrt_fd=0;
  __SYS_CALL_TEST_NM1_RETURN((out_fd=socket(AF_INET, 
                                        SOCK_STREAM, 
					 0)));

  
  sockaddr_in addr_in;
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = 9898;

  hostent* phost=0;
  
  if((hostent*) 0 ==  (phost=gethostbyname(HOST)))
  {
    printf("bad host name %s\n", HOST);
    exit(1);
  }
  
  memcpy(&addr_in.sin_addr, &phost->h_addr, sizeof(addr_in.sin_addr));

  __SYS_CALL_TEST_NM1_RETURN( (wrt_fd=connect(out_fd, 
                                      (struct sockaddr*)&addr_in, 
				       sizeof(addr_in))) );

  if(0 > wrt_fd) printf("connect failed\n");

  // write something
  __WRITE_FD_DRAIN("wow such joy", wrt_fd, 12);
  return 0;
}
