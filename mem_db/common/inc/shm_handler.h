#ifndef __MEM_DB_COMMON_INC_SHM_HANDLER_H_
#define __MEM_DB_COMMON_INC_SHM_HANDLER_H_
#include <sys/shm.h> // for shm defs
#include "types.h" // For my types
#include "shr/macros.h" // my beloved macros


class mem_allocator
{


public:
  /*TODO: sync the access to the alloc*/
  static mem_allocator& get_alloc()
  {static mem_allocator s_alloc;return s_alloc;}




private:


  // assumption is that:
  // no allocation will ever exceed 1gb[2^30 bytes]
  // in the worst case, entire chunk is alloc'ed, thats
  // why the nxt/prv need to support these sizes as well
  typedef struct tChunkHeader
  {
    fw::uint32 sz;
    fw::uint32 actSz;
    fw::uint16 nxt; // one bit is used for occupied
    fw::uint16 prv; // only 15 bits available
  }ChunkHeader;


  static const fw::uint32 ALLOC_MASK = 0x00000001;
  mem_allocator();


  /*
  * the following are funcs manipulating the
  * pools internal state. 
  */
  inline static unsigned int 
  getSz(void* pshm)
  {
    ChunkHeader* phead; 
    __RE_INTPRT_MEM_TO_PTYPE(phead, ChunkHeader, pshm);
    phead--;
    __PRINT_ADDR(phead);
    return phead->sz;
  }


 inline static void updateSz(unsigned int i_sz, void* pshm)
  {
    ChunkHeader* phead =
    __RE_INTPRT_MEM_TO_PTYPE(ChunkHeader, pshm);
    phead--;
    phead->sz+=i_sz;
  } 


   inline static unsigned int 
  getActSz(void* pshm)
  {
    ChunkHeader* phead;
    __RE_INTPRT_MEM_TO_PTYPE(phead, ChunkHeader, pshm);
    phead--;
    return phead->nxt >> 1;
  }
  
  inline static void updateActSz(unsigned int i_sz, void* pshm)
  {
    ChunkHeader* phead;
    __RE_INTPRT_MEM_TO_PTYPE(phead, ChunkHeader, pshm);
    phead--;
    phead->actSz+=i_sz;
    phead->actSz=(actSz << 1) | (phead->actSz) & 0x00000001;
  }


  ~mem_allocator(){}


  int initAllocator();
  
  typedef mem_db::Uint32 Pointer;
  static const mem_db::Uint32  NULL_POINTER;


  int disposeChunk(const Pointer i_pointer);

  void* pointerToLocalPointer(const Pointer& pointer);

 
  void* reAllocSHM(Pointer& pointer);


   void* allocChunk(const unsigned int i_Sz, Pointer& o_pointer);
 private:

   int getNextFreeBlock(Pointer&);
  
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
