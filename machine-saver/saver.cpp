/*
* this server has a very simple role - save machines that got bitten by
* the obit bad module.
*/

/*
* class Saver
* used to save the locked machine
*/
#include<>


class Saver 
{
private:

  /*
  * listening socket
  */
  int m_socket;

public:

/*   we allow 2 forms of construction:
     1. using a valid ip(7) address
     2. using a hostname in a valid format. this form requires that the
        server be registered in one of the way acceptable by gethostbyname(3)
*/

  /*
  * C'tor
  */
  Saver(const char* const ip_hostNm)
  {
    
  }

  /*
  * D'tor
  */
  ~Saver();

  /*
  * listens to events
  */
  listen2Events();
  
};
