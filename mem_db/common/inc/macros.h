#ifndef __MEM_DB_COMMON_MACROS_H_
#define __MEM_DB_COMMON_MACROS_H_

#define __SYS_CALL_TEST_RETURN(EXP) \
  if(0 > (EXP)) \
  { \
    perror(__func__); \
    return -1; \
  }

#define __SYS_CALL_TEST_RETURN_NULL(EXP) \
  if(0 > (EXP)) \
  { \
    perror(__func__); \
    return (void*)0;	      \
  }

#define __SYS_CALL_TEST(EXP) \
  if(0 > (EXP)) \
    perror(__func__);



#define __SYS_CALL_TEST_EXIT(EXP) \
  if(0 > (EXP)) \
  { \
    perror(__func__); \
    exit(1); \
  }

#define __RE_INTPRT_SHM_TO_TYPE(TYP,SHM) \
  reinterpret_cast<TYP*>(SHM)

 #define __CAST_SHM_TO_TYP_P(TYP,S_MEM,ALLOC) \
   reinterpret_cast<TYP*>(ALLOC.pointerToLocalPointer(S_MEM));



#define __INIT_SHM_POINTER(PNT) \
  PNT.key=0; \
  PNT.offset=0;

#define __INIT_CHUNK_HDR(HDR) \
  HDR->sz=0; \
  HDR->actSz=0;

#define __START_BLIND_PRINT static unsigned int _count=0; 
#define _P printf("print [%u]\n", _count++);

#ifdef __DEBUG
#define __CHECK_ERRNO(ERR,MSG) \
  if(ERR == errno) printf("%s\n", MSG);
#else
#define __CHECK_ERRNO(ERR,MSG)
#endif // __DEBUG

#define __PRINT_ADDR(PNT) \
  printf("pointer addr is [%p] in func [%s]\n", PNT, __func__);


#endif //  __MEM_DB_COMMON_MACROS_H_
