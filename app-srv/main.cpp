/*
* This is an attempt at building an obit application srv.
*/
#include "acceptor.h"

int
main(int argc, char* argv[])
{
  app_srv::nw::acceptor acc;
  char ch; /* not used for now*/
  acc.init(&ch);
  acc.listen_2_events();
  
  return 0;
}

