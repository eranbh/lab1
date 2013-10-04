/*
* This is he basic multi core task. Any code
* extending this class needs to take care
* of 3 hooks:
* 1. init - iniialization at thread level
* 2. run  - executes task logic [thread safeness
*           is under the responsibilty of the user]
* 3. fini - cleans up at thread level
*
* Any implementation may assign threads to cores as
* it sees fit - with the constraint of user prevliges
*
* The code here depends on the pthread implementation
* available with the distribuion on your machine. Make
* sure to compile and link with the proper resources   
*/
#include <stdio.h>
#include <pthread.h> // For threading

namespace fw{

class MultiCore_Task
{
 public:
  friend void* launcher(void* arg);
  // Any task level initializations
  MultiCore_Task(unsigned int i_thNum=1);

  virtual ~MultiCore_Task(){}

  // activates the entire task
  int activate();

  // Task level de activation
  int deActivate();


  int waitForAll();

 protected:
  // All need to be overriden in 
  //higher layer by user applications

  virtual int init(){return 0;}
  // void* run_i(void* args){init();/*run();fini();*/}
  virtual int run(){printf("in basic task\n");return 0;}
  virtual int fini(){return 0;}

 private:
  unsigned int m_thNm;
  pthread_t m_pthHandleArr[10];
  //pthread_t m_pthHndl;
};

} // namespace fw
