
#include "Task.h"

namespace fw {
MultiCore_Task::
MultiCore_Task(unsigned int i_thNm):m_thNm(i_thNm)
{
}

void* launcher(void* arg)
{
  MultiCore_Task* task = (MultiCore_Task*)arg ;
  // Error handling should be done on impl  level
  task->init();
  task->run();
  task->fini();
}

int MultiCore_Task::activate()
{
  //pthread_t* m_pthHandleArr = new pthread_t[10];
  for(unsigned int i=0 ; i<m_thNm ; ++i)
    {
     // check sts

    pthread_create(m_pthHandleArr+i,
		   /*pthread_attr_t for now*/0,
		   launcher, (void*) this);
    printf("creating handle %lu\n", m_pthHandleArr[i]);
    }
  return 0;
}

/*Waits for ALL threads on task*/
int MultiCore_Task::waitForAll()
{
  void* ret[m_thNm]; // ONLY ON GCC!!
  for(unsigned int i=0 ; i<m_thNm ; ++i)
    {
          printf("join handle %lu\n", m_pthHandleArr[i]);
     // check sts
   pthread_join(m_pthHandleArr[i], ret /*void **retval for now*/);
    }
   return 0;
}

int MultiCore_Task::deActivate()
{
  // Cancel threads
}

} // namespace fw

/*int
main(int argc, char* argv[])
{
  MultiCore_Task task;
  task.activate();
  task.waitForAll();
  return 0;
}*/
