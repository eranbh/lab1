#ifndef __NW_INC_ACCEPTOR_H_
#define __NW_INC_ACCEPTOR_H_

/*
* This module is responsible for handling all the interaction with clients
* over the network. This is achieved by means of multiplexing the
* communication of several clients to one comm handler using epoll(7)
*/


namespace mem_db{
  namespace nw{

class acceptor
{
public:
  
  /*
  * C'tor
  */
  acceptor(){}

  /*
  * D'tor
  */ 
  virtual ~acceptor(){}

  /*
  * sets up the multiplexing
  */
  int init(const char* const ip_ip);

  
  /*
  * This function implements the actual listening
  */
  int listen_2_events();

private:

  /*
  * handles requests on incoming conn's  
  */
  int handle_request(int fd);

  int m_listen_fd; 
  int m_epoll_fd;
  
};

  } // namespace nw
} // namespace mem_db


#endif // __NW_INC_ACCEPTOR_H_
