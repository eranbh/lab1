/*****************************
* SHM control and monitor tool
******************************/
#include <stdio.h> // For printf
#include <stdlib.h> // For exit(3)
#include <sys/ipc.h> // for shm defs
#include <sys/shm.h> // for shm defs
#include "macros.h" // For macros
#include "shm_handler.h"

static void run_user_loop();
static int handle_opt(unsigned short i_opt);
static int handle_disply();
static int handle_delete();
static int get_num_of_seg();
static int handle_delete_by_seg(unsigned int);
static unsigned int s_num_of_seg=0;

int main(int argc, char* argv[])
{
  run_user_loop();
  return 0;
}

void run_user_loop()
{
  unsigned short opt=0;
  get_num_of_seg();
  do
  {
    printf("select from the following please:\n");
    printf("1. display segments\n");
    printf("2. delete segments\n");
    printf("3. delete segment by id\n");
    printf("4. amount of objects in db\n");
    printf("0. exit ...\n");
       
  }while(0 !=  scanf("%hu", &opt) &&
         0 != opt                 &&
         -1!= handle_opt(opt));
}

int handle_opt(unsigned short i_opt)
{
  switch(i_opt)
  {
      case 1: return handle_disply();
      case 2: return handle_delete();
      case 3: 
	{
	   printf("segment number to delete:");
           unsigned int opt;
	   if(0 == scanf("%u", &opt) ||
	      0 > opt)
	     {
	       printf("invalid segment number\n");
	       return 0;
	     }
	     return handle_delete_by_seg(opt);
	}
      case 4: return get_num_of_seg();
      default: 
	{
	  printf("\n\nBAD OPTION. TRY AGAIN\n\n");
          return 0;
	}
  }
  return -1; // Shouldnt get to this ..
}

int handle_disply()
{

  int id=0;
   for(unsigned short i=2;i<2+s_num_of_seg;++i)
   {
     printf("releasing id: %u\n",i);
      int id=0;
      __SYS_CALL_TEST_RETURN(id=shmget(i, 
                 		   0,
			           0666))

     void* pshm=0;
      __SYS_CALL_TEST_RETURN(pshm=shmat(id, 
                             0, 
             	             0666))
     
     printf("segment: %hu size: %u act: %u\n", 
            i,
            mem_allocator::getSz(pshm), 
            mem_allocator::getActSz(pshm));
   }
  
  return 0;
}

int handle_delete()
{
  int id=0;

  for(unsigned short i=1;i<=1+s_num_of_seg;++i)
  {   
    int ret=0;
    if(0 != (ret = handle_delete_by_seg(i))){return ret;}
  }

  return 0;
}

int get_num_of_seg()
{
  int id=0;
  __SYS_CALL_TEST_RETURN(id=shmget(1, 
                 		   0,
			           0666))

   void* pshm=0;
  __SYS_CALL_TEST_RETURN(pshm=shmat(id, 
                               0, 
			       0666))
    
   s_num_of_seg = 
    *(__RE_INTPRT_SHM_TO_TYPE(unsigned int, pshm));

    printf("current amount of segments %u\n",  s_num_of_seg);
    return 0;
}

int handle_delete_by_seg(unsigned int i_opt)
{
  int id=0;
  __SYS_CALL_TEST_RETURN(id=shmget(i_opt, 
                 		   0,
			           0666))

   void* pshm=0;
  __SYS_CALL_TEST_RETURN(pshm=shmat(id, 
                               0, 
			       0666))

  __SYS_CALL_TEST_RETURN(shmctl(id, IPC_RMID, 0))

  __SYS_CALL_TEST_RETURN(shmdt(pshm))// Make sure a delete happens

    printf("deleted seg %u\n", i_opt);
  
  return 0;
}
