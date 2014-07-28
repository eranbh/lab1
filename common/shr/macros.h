#ifndef __COMMON_SHR_MACROS_H_
#define __COMMON_SHR_MACROS_H__

#include <errno.h> // for errno(3)
#include <stdlib.h> // for exit(3)
#include <unistd.h> // for write(2)
#include <assert.h> // for assert(3)

#define __SYS_CALL_TEST_NM1_RETURN(EXP)   \
  do                                      \
  {                                       \
     errno=0;                             \
     if(0 > EXP)	                  \
     { perror(#EXP); return -1;  }        \
  }while(0)


#define __SYS_CALL_TEST_NM1_EXIT(EXP)     \
  do                                      \
  {                                       \
     errno=0;                             \
     if(0 > EXP)	                  \
       { perror(#EXP); exit(1);  }	  \
  }while(0)



#define __SYS_CALL_TEST_RETURN_NULL(EXP) \
  if(0 > (EXP))                          \
    {                                    \
  perror(__func__);                      \
  return (void*)0;                       \
    }



#define __RE_INTPRT_MEM_TO_PTYPE(RES,TYP,MEM)	\
do                                              \
{                                               \
  assert(MEM);                                  \
  RES=*reinterpret_cast<TYP*>(MEM);		\
}while(0)


#define __STRINGIFY(W) #W

#include <stdio.h>
#define __DRAIN_LOOP(FD,BUFF,ACT)  			              \
  while( ((tot_tmp+=tmp)<len) &&				      \
         (0<(tmp=ACT(FD, BUFF+tot_tmp, len - tot_tmp))))              \
    {printf("looping\n");};					      \
  if(0 > tmp) { perror(__STRINGIFY(ACT)); exit(3);}                           

#define __DRAIN_VARS(LEN) int tmp=0, tot_tmp=0, len=LEN;

#define __READ_FD_DRAIN(BUFF,FD,LEN)				      \
do                                                                    \
{                                                                     \
 __DRAIN_VARS(LEN)                                                    \
  __DRAIN_LOOP(FD,BUFF,read)					      \
}while(0)

#define __WRITE_FD_DRAIN(BUFF,FD,LEN)				      \
do                                                                    \
{                                                                     \
  __DRAIN_VARS(LEN)                                                   \
  __DRAIN_LOOP(FD,BUFF,write)					      \
}while(0)



#define __START_BLIND_PRINT static unsigned int _count=0;
#define _P printf("print [%u]\n", _count++);

#ifdef __DEBUG
#define __CHECK_ERRNO(ERR,MSG)		\
  if(ERR == errno) printf("%s\n", MSG);
#else
#define __CHECK_ERRNO(ERR,MSG)
#endif // __DEBUG                                                                                                                                                                                        

#define __PRINT_ADDR(PNT) \
  printf("pointer addr is [%p] in func [%s]\n", PNT, __func__);



#endif // __COMMON_SHR_MACROS_H_
