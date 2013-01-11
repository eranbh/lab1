// Tests all network interfaces
// Focuses on the server side
#include <stdio.h> // For printf
#include <stdlib.h> // For exit(3)
#include "network_interface.h"

static int test_tcp_blocking_acceptor();
static void prepareEnv(int argc, char* argv[]);
static void usage();
static unsigned long s_port;
int main(int argc, char* argv[])
{
  prepareEnv(argc, argv);
  test_tcp_blocking_acceptor();
  return 0;
}


///// IMPLLEMENTATION ////


int test_tcp_blocking_acceptor()
{
    BlockingTcpAcceptor acceptor(s_port);
    mem_db::NetworkMsg msg;
    return acceptor.listen2Events(msg);
}

void prepareEnv(int argc, char* argv[])
{
  if(2 > argc)
    usage();
  char buff[16];
  char* pbuff = buff;
  s_port = strtol(argv[1], &pbuff, 10);

  if(*pbuff != '\0')
  {
    printf("port not a valid numbrt %s\n", argv[1]);
    usage();
  }
}

void usage()
{
  printf("USAGE: COM_RCV_TEST <port number> [buffer size]\n");
  exit(1);
}
