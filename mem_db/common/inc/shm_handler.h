#ifndef __MEM_DB_COMMON_INC_SHM_HANDLER_H_
#define __MEM_DB_COMMON_INC_SHM_HANDLER_H_
#include <sys/shm.h> // for shm defs
#include "types.h" // For my types
#include "shr/macros.h" // my beloved macros


class mem_allocator
{
private:


  typedef struct tChunkHeader
  {
    unsigned int sz;
    unsigned int actSz;
  }ChunkHeader;


  static const unsigned int SMALL_CHUNK_SZ = 1024;
  static const unsigned int HUGE_CHUNK_SZ  = 4096;

  mem_allocator();

public:


  /*TODO: sync the access to the alloc*/
  static mem_allocator& get_alloc()
  {static mem_allocator s_alloc;return s_alloc;}


  // Assumes valid memory
  inline static unsigned int 
  getSz(void* pshm)
  {
    ChunkHeader* phead = 
    __RE_INTPRT_MEM_TO_PTYPE(ChunkHeader, pshm);
    phead--;
    __PRINT_ADDR(phead);
    return phead->sz;
  }

   inline static unsigned int 
  getActSz(void* pshm)
  {
    ChunkHeader* phead = 
    __RE_INTPRT_MEM_TO_PTYPE(ChunkHeader, pshm);
    phead--;
    return phead->actSz;
  }
  
  inline static void updateActSz(unsigned int i_sz, void* pshm)
  {
    ChunkHeader* phead =
    __RE_INTPRT_MEM_TO_PTYPE(ChunkHeader, pshm);
    phead--;
    phead->actSz+=i_sz;
  } 

  inline static void updateSz(unsigned int i_sz, void* pshm)
  {
    ChunkHeader* phead =
    __RE_INTPRT_MEM_TO_PTYPE(ChunkHeader, pshm);
    phead--;
    phead->sz+=i_sz;
  } 


  ~mem_allocator(){}


  int initAllocator();
  
  typedef mem_db::Uint32 Pointer;
  static const mem_db::Uint32  NULL_POINTER;


  int disposeChunk(const Pointer i_pointer);

  void* allocSmallChunk(Pointer& o_pointer)
  {return allocChunk(SMALL_CHUNK_SZ, o_pointer);}
  void* allocHugeChunk(Pointer& o_pointer)
  {return allocChunk(HUGE_CHUNK_SZ, o_pointer);}

  void* pointerToLocalPointer(const Pointer& pointer);

  // This actually:
  // 1. creates a new SHM key
  // 2. Allocates a new [bigger] chunk
  // 3. copies the data on to the allocated mem
  // 4. Changes the db_start shm key with the new one
  void* reAllocSHM(Pointer& pointer);


   void* allocChunk(const unsigned int i_Sz, Pointer& o_pointer);
 private:
  
   /*
   * the shm chunk's id 
   */
   int m_id;

   /*
   * the vmem pointer for root
   * caution: NEVER save this on shm
   */
   void* mp_vmem;
  
};



#endif // __MEM_DB_COMMON_INC_SHM_HANDLER_H_
