#ifndef __APP_SRV_COMMON_INC_MACROS_H_
#define __APP_SRV_COMMON_INC_MACROS_H__

#define __SYS_CALL_TEST_NM1_RETURN(EXP) \
  do \
  { \
     if(0 > EXP)	\
     { perror(#EXP); return -1;  } \
  }while(0)



#define __STRINGIFY(W) #W

#define __DRAIN_LOOP(FD,BUFF,ACT)  			              \
  while( (0 > (tmp=ACT(FD, BUFF+tot_tmp, len - tot_tmp))) &&          \
         (len > (tot_tmp+=tmp)) );                                    \
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



#endif // __APP_SRV_COMMON_INC_MACROS_H_
