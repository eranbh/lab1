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
* The code here depends on the threading implementation 
* available with the distribuion on your machine. Make
*/

namespace fw{

template<typename USER_TYPE>
class MultiCore_Task
{
 public:
  
  // Any task level initializations
  MultiCore_Task(unsigned int i_thNum=1);

  virtual ~MultiCore_Task(){}

  // activates the entire task
  int start();

  // Task level de activation
  int stop();


 protected:
  
  // All need to be overriden in 
  //higher layer by user applications
  virtual int init(){return 0;}
  virtual int run(){printf("in basic task\n");return 0;}
  virtual int fini(){return 0;}

 private:
  unsigned int m_thNm;
  pthread_t m_pthHandleArr[10];
};

} // namespace fw
