#ifndef __NW_INC_ACCEPTOR_H_
#define __NW_INC_ACCEPTOR_H_
#include <unistd.h>
/*
* This module is responsible for handling all the interaction with clients
* over the network. This is achieved by means of multiplexing the
* communication of several clients to one comm handler using epoll(7)
*/

namespace nw{

  namespace ut{
    class nwUT;
  }

class Acceptor
{
public:
  
  friend class nw::ut::nwUT;
  
  /*
  * C'tor
  */

  Acceptor(const char* const, int i_backlog = 10);

  /*
  * D'tor
  * Stopping the loop here is done only for completeness
  * As the wait on any thread execing this clss will be performed
  * outside of this class, instances on the stack will have to be
  * stopped by calling stopAcc directly
  */ 
  virtual ~Acceptor()
    {
      close(m_listen_fd);
      close(m_epoll_fd);
    }

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
  * we allow customization of the entire handler
  */
  virtual int handle_request(int fd);


  int m_listen_fd; 
  int m_epoll_fd;

};

} // namespace nw


#endif // __NW_INC_ACCEPTOR_H_
