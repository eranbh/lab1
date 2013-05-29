/*
* this server has a very simple role - save machines that got bitten by
* the obit bad module.
*/

/*
* class Saver
* used to save the locked machine
*/
class Saver 
{
private:

  /*
  * listening socket
  */
  int m_socket;

public:
  
  /*
  * C'tor
  */
  Saver();

  /*
  * D'tor
  */
  ~Saver();

  /*
  * listens to events
  */
  listen2Events();
  
};
