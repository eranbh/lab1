/***************************************
* Basic shared memory handler does NO 
* caching of the memory allocated
***************************************/
#include <stdio.h> // For perror
#include <sys/ipc.h> // for shm defs
#include <string.h> // For memset
#include <fcntl.h>  // for open(2)
#include <assert.h>
#include "shr/types.h" // For my types
#include "shm_handler.h"

const mem_db::Uint32 mem_allocator::NULL_POINTER=0;
const mem_db::Uint32 MEGA = 1024*1024;


#define __INIT_CHUNK_HEAD(CHNK) \
do{                             \
  CHNK.sz=0;                    \
  CHNK.actSz=0;                 \
}while(0)

/*
* C'tor  
=======
* this c'tor allocates a blank chunk of mem to
* get the ball rolling. any error here is fatal
*/
mem_allocator::mem_allocator()
{

   __SYS_CALL_TEST_NM1_EXIT((m_id=shmget(1,
				      MEGA,
				      IPC_CREAT|0666|IPC_EXCL)));
      
  __SYS_CALL_TEST_NM1_EXIT((mp_vmem=shmat(m_id, 
				         0, 
				         0666)));

  assert(0 < mp_vmem && "bad vmem from shmat");
  // we have a fresh block - lets clean it
  memset(mp_vmem, 0, MEGA);

  // now export the id to the world
  int fd;
  __SYS_CALL_TEST_NM1_EXIT((fd=open("id",
				    O_CREAT|O_SYNC,
				    S_IRWXU)));

  __WRITE_FD_DRAIN(&m_id, fd, sizeof(int));

  // because we specified O_SYNC, the id
  // is garenteed to be on the disk now

  // place chunk header

  ChunkHeader* pchunkHeader = 
    __RE_INTPRT_MEM_TO_PTYPE(ChunkHeader, mp_vmem);

  pchunkHeader->sz=MEGA;
  pchunkHeader->actSz=0;
}

int mem_allocator::initAllocator()
{
  return 0;
}


void* mem_allocator::
allocChunk(const unsigned int i_Sz, Pointer& o_pointer)
{
  // TODO access free block list and fetch block
  //      check if allocation can be satisfied
  //      for now, always give the mem in the beginig
  //      of the allocated block
  //      first fill in header
  ChunkHeader* pchunkHeader =
    __RE_INTPRT_MEM_TO_PTYPE(ChunkHeader, mp_vmem);

  pchunkHeader->actSz+=i_Sz;

  // TODO make sure we sync any cpu caches here!

  o_pointer = i_Sz;
  return static_cast<char*>(mp_vmem)+i_Sz;
}

inline void* mem_allocator::
pointerToLocalPointer(const Pointer& pointer)
{
  return static_cast<char*>(mp_vmem)+pointer;
}

void* mem_allocator::reAllocSHM(Pointer& io_pointer)
{ 
  return 0;
}


int mem_allocator::
disposeChunk(const Pointer i_pointer)
{
 
  // send the chunk back to the free list
  __SYS_CALL_TEST_NM1_RETURN(shmctl(m_id, IPC_RMID, 0));

 
  __SYS_CALL_TEST_NM1_RETURN(shmdt(mp_vmem));

  // TODO update dictionary

  return 0;
}


