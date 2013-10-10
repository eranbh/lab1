/*
* this server has a very simple role - save machines that got bitten by
* the obit bad module.
*/

/*
* class Saver
* used to save the locked machine
*/
#include "Task.h"
#include "Acceptor.h"

/*
* This class is a simple task that runs to infinity and performs:
* 1. listen on an agreed upon port for a message
* 2. is msg type
*    2.1 smart shell - open a _very_ basic shell that ignores preloading
*        [this might not be possible on all platforms]
*    2.2 run an arbitrary exec
*    2.3 perform a "save" operation. this is impl specific:
*        2.3.1 on linux [give further details]
*        2.3.2 on bsd [give further details]
*        2.3.3 on ibm [give further details]
*        2.3.4 on hp [give further details]
*/
class Saver : public MultiCore_Task
{
public:

  /*
  * C'tor
  */
  Saver()
  {

  }

protected:

  virtual int init();
  virtual int run();
  virtual int fini();

  /*
  * D'tor
  */
  virtual ~Saver(){}

private:

  /**/
  nw::Acceptor m_acc;
};


int Saver::init()
{
	return 0;
}

int Saver::run()
{
	return 0;
}

int Saver::fini()
{return 0;}



