/***************************************
* Basic shared memory handler does NO 
* caching of the memory allocated 
***************************************/
#include <stdio.h> // For perror
#include <sys/ipc.h> // for shm defs
#include <string.h> // For ::memcpy
#include <assert.h>
#include "macros.h" // For my macros
#include "types.h" // For my types
#include "shm_handler.h"

const mem_db::Uint32 mem_allocator::NULL_POINTER=0;


mem_allocator::mem_allocator(): m_dbStart(1)
{

   int id=0;
  id=shmget(m_dbStart,
            sizeof(unsigned int),
	    IPC_CREAT|0666|IPC_EXCL);
  assert(-1 != id);
  void* pshm=0;
  pshm=shmat(id, 
             0, 
             0666);
  assert((void*)-1 != pshm);
  

   (*(__RE_INTPRT_SHM_TO_TYPE(unsigned int,pshm)))=0;
}

int mem_allocator::initAllocator()
{
  return 0;
}


#define __INIT_CHUNK_HEAD(CHNK) \
  CHNK.sz=0; \
  CHNK.actSz=0;


void* mem_allocator::
allocChunk(const unsigned int i_Sz, Pointer& o_pointer)
{
  int id=0;
  printf("in allocator, allocating sz %u\n", i_Sz);

  __SYS_CALL_TEST_RETURN_NULL(id=shmget(++m_dbStart, 
				   i_Sz+sizeof(ChunkHeader),
				   IPC_CREAT|0666|IPC_EXCL))
    void* pshm=0;
  __SYS_CALL_TEST_RETURN_NULL((pshm=shmat(id, 
                                          0, 
					  0666)))

   ChunkHeader* phead = 
    __RE_INTPRT_SHM_TO_TYPE(ChunkHeader,pshm);
   
  __PRINT_ADDR(phead)
  
  __INIT_CHUNK_HDR(phead);
  phead->sz=i_Sz;
  printf("sz requested from allocator: %u\n",phead->sz);

   // UPDATING START - THIS SHOULD BE CHANGED
   __SYS_CALL_TEST_RETURN_NULL(id=shmget(1, 
			                 0,
				         0666))

  __SYS_CALL_TEST_RETURN_NULL((pshm=shmat(id, 
                                          0, 
					  0666)))
  
   o_pointer=m_dbStart;
   
   __PRINT_ADDR(phead + 1)
   
   return phead + 1;
}

void* mem_allocator::
pointerToLocalPointer(const Pointer& pointer)
{

  int id=0;
  __SYS_CALL_TEST_RETURN_NULL(id=shmget(pointer, 0,0666))
    void* pshm=0;
  __SYS_CALL_TEST_RETURN_NULL(pshm=shmat(id,
                                    0,
			            0666))

    return  (__RE_INTPRT_SHM_TO_TYPE(char,pshm)) + 
                           sizeof(ChunkHeader); 
}

void* mem_allocator::reAllocSHM(Pointer& io_pointer)
{

  int id=0;
   __SYS_CALL_TEST_RETURN_NULL(id=shmget(io_pointer, 
			              0,
			              0))

    void* pshm=0;
  __SYS_CALL_TEST_RETURN_NULL(pshm=shmat(id, 
                                    0, 
			            0666))

  ChunkHeader& oldheader = 
    *(__RE_INTPRT_SHM_TO_TYPE(ChunkHeader, pshm));

    // Allocate new sz - double the size
    int newId=0;
  __SYS_CALL_TEST_RETURN_NULL(newId=shmget(++m_dbStart, 
			           oldheader.sz*2,
			           0666|IPC_CREAT|IPC_EXCL ))

    void* pNewShm=0;
  __SYS_CALL_TEST_RETURN_NULL(pNewShm=shmat(newId, 
                                    0, 
			            0666))

  ChunkHeader& head =
    *(__RE_INTPRT_SHM_TO_TYPE(ChunkHeader,pNewShm));
  head.sz*=2;
  head.actSz=oldheader.actSz;
  char* pbytes = 
    __RE_INTPRT_SHM_TO_TYPE(char, pNewShm);
  ::memcpy(pbytes+sizeof(ChunkHeader), pshm, oldheader.actSz);


  // Get rid of the old memory
  __SYS_CALL_TEST_RETURN_NULL(shmctl(id, IPC_RMID, 0))

    io_pointer=m_dbStart;
  
  return pNewShm;
}

// This version assumes u have a virtual pointer too
int mem_allocator::
disposeChunk(const Pointer i_pointer)
{
   int id=0;
   __SYS_CALL_TEST_RETURN(id=shmget(i_pointer, 
			            0,
			            0666))

    void* pshm=0;
  __SYS_CALL_TEST_RETURN(pshm=shmat(id, 
                                    0, 
			            0666))

   // Only after all processes detach
  // is this chunk of memory actually destroyed
  __SYS_CALL_TEST_RETURN(shmctl(id, IPC_RMID, 0))

    // server detaches - is there anyone else ??
  __SYS_CALL_TEST_RETURN(shmdt(pshm))


   // UPDATING START - THIS SHOULD BE CHANGED
   __SYS_CALL_TEST_RETURN(id=shmget(1, 
			           0,
				   0666))
 
  __SYS_CALL_TEST_RETURN((pshm=shmat(id, 
                                     0, 
			             0666)))

  unsigned int* pstart = 
   __RE_INTPRT_SHM_TO_TYPE(unsigned int,pshm);
   (*pstart)--  ;

 
  return 0;
}


