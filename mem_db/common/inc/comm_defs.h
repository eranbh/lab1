#ifndef _COMMON_INC_COMM_DEFS_H_
#define _COMMON_INC_COMM_DEFS_H_
/*
* This H contains a bunch of usfull macros, functions
* and definitions that can be used throughout the code
*/

#include <errno.h> // For perror

#define __TEST_SET_SYS_CALL_RETURN(TERM) \
  int ret=0; \
  if(0 > (ret=TERM)) \
  { \
     perror("Error while in" __FUNCTION__ " " __LINE__); \
     return ret;
  }

#endif // _COMMON_INC_COMM_DEFS_H_
