#include <stdlib.h> // For exit(3)
#include <stdio.h> // for printf
#include "macros.h" // For my macros
#include "shm_handler.h" // Tested code
static mem_allocator* s_palloc=new mem_allocator;
int run_suite();
// Test cases included in this package
static int test_simple_small_chunk();
static int test_simple_huge_chunk();
static int test_reAlloc_chunk();
static int test_dispose_chunk();
// TODO move this to CPPUNIT!!

#define __WAIT while(1);

int main(int argc, char* argv[])
{
  // Prepare env
  if(0 != run_suite())
  {
    printf("Error in testing. Exiting\n");
    exit(1);
  }
  delete s_palloc;
  return 0;
}

int run_suite()
{
  __SYS_CALL_TEST_RETURN(test_simple_small_chunk())
  __SYS_CALL_TEST_RETURN(test_simple_huge_chunk())
    //test_reAlloc_chunk();
   __SYS_CALL_TEST_RETURN(test_dispose_chunk())

  return 0;
}

#define __ASSERT_EQUAL_MESSAGE(EXP,MSG)		\
  if(!(EXP)) printf(MSG);


int test_simple_small_chunk()
{
  mem_allocator::Pointer pointer;
  
  void* pshm = 
    s_palloc->allocSmallChunk(pointer);
  __ASSERT_EQUAL_MESSAGE(pshm != 0, "test_simple_small_chunk\n")
  __ASSERT_EQUAL_MESSAGE(pointer== 2, "test_simple_small_chunk\n")
  
  return 0;
}
int test_simple_huge_chunk()
{


  mem_allocator::Pointer pointer;
  void* pshm = 
    s_palloc->allocHugeChunk(pointer);
  __ASSERT_EQUAL_MESSAGE(pshm != 0, "test_simple_huge_chunk\n")
    __ASSERT_EQUAL_MESSAGE(pointer == 3, "test_simple_huge_chunk\n")
 
  return 0;
}
int test_reAlloc_chunk()
{
  mem_allocator::Pointer pointer;
  void* pshm = 
    s_palloc->allocHugeChunk(pointer);
  __ASSERT_EQUAL_MESSAGE(pshm != 0, "test_reAlloc_chunk\n")
  __ASSERT_EQUAL_MESSAGE(pointer== 4, "test_reAlloc_chunk\n")
 
    printf("reallocating for key %u\n", pointer);

  // reallocating
   s_palloc->reAllocSHM(pointer);
  __ASSERT_EQUAL_MESSAGE(pshm != 0, "test_reAlloc_chunk\n")
  __ASSERT_EQUAL_MESSAGE(pointer == 5, "test_reAlloc_chunk\n")
  
 
  return 0;
}
int test_dispose_chunk()
{
  mem_allocator::Pointer pointer;
  pointer=3;
  __ASSERT_EQUAL_MESSAGE(-1 != s_palloc->disposeChunk(pointer),
                         "test_dispose_chunk")
  
  return 0;
}
