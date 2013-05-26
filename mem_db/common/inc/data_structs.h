/*
* This file contains the basic data structures
* These structures respect data consistancy constraints
* They use a journal in order to document the activities
* and supprt recovery for any data that might be half baked
* The db uses the following data structures:
* 1. bit_maps
* 2. stacks - lifo Qs
* 3. Qs - need to be lock less!!! [rcu]
*/
#include <assert.h>
#include "types.h"

// TODO is this ok here?
const unsigned short MAX_OBJ_NUM=32;
#define INV_LOCATION ((unsigned int)-1)

// TODO consider failpoints for testing

typedef struct tStack
{
  friend class DataStructsUT;
  tStack(){}
  void init(unsigned int i_sz)
  { 
    assert(i_sz <= MAX_OBJ_NUM);
    sz=i_sz;
    capacity=0;
  }

  // Make sure the below are atomic!!
  inline unsigned int push(unsigned int i_slot)
  {
    assert(capacity<=sz);
    assert(capacity>=0);
    if(capacity < sz) // frequently
    {
       pstack[capacity++]=i_slot;
       return 0;
    }
    return INV_LOCATION;
  }

  inline unsigned int pop()
  {
    assert(capacity<=sz);
    assert(capacity>=0);
    if(0 < capacity)
       return pstack[--capacity];
    return INV_LOCATION;
  }
  void print_stack()
  {
    for (unsigned int i=0 ; i<capacity ; ++i)
      printf("stack val: %u\n", pstack[i]);
  }
 
  inline unsigned int getSz()const {return sz;}
  inline unsigned int getCapacity()const {return capacity;}
  
  inline void clear()
  {
    capacity=0;
  }
 
private:


  unsigned int sz;
  unsigned int capacity;
  // using ints to support large data sets
  unsigned int pstack[MAX_OBJ_NUM];

}Stack;


//////////////////////////////////////////////////////////////////

#define __
const unsigned short MIN_BIT=0x1;
const unsigned short MAX_BIT=0x31; // the highest bit possible is 31
  // for testing
class DataStructsUT;

typedef struct tBitMap
{

   friend class DataStructsUT;
  tBitMap():bits(0){}
  void init(){bits=0;}
  void bit_on(mem_db::Uint32 bitNum)
  {
    assert(MIN_BIT <= bitNum && MAX_BIT >= bitNum);
    bits |= 1 << (bitNum-1);
  }

  void bit_off(mem_db::Uint32 bitNum)
  {
    assert(MIN_BIT <= bitNum && MAX_BIT >= bitNum);
    bits &= ~(1 << (bitNum-1));
  }

  bool is_on(mem_db::Uint32 bitNum)
  {
     assert(MIN_BIT <= bitNum && MAX_BIT >= bitNum);
    return bits & (1 << (bitNum-1));
  }

  mem_db::Uint32 bits;
}BitMap;
