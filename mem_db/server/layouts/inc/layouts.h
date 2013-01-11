#ifndef __MEM_DB_SERVER_LAYOUTS_INC_LAYOUTS_H_
#define __MEM_DB_SERVER_LAYOUTS_INC_LAYOUTS_H_
/*************************************************************************
* Defines the layouts available for the server
* The main purpose of a layout is to manager a
* piece of shared memory. This memory can have
* various properties [size, dynamic growth etc]
* layout offered:
* 1. simple byte array - small static chunks where direct access is needed
* 2. linked list - small/big dynamically growing chunks where no direct
*                  access is needed
* 3. hash - big chunks where fast access is needed
***************************************************************************/
#include<limits.h> // for UINT_MAX
#include <stdio.h> // for printf
#include <string.h>  // for memcpy/memcmp
#include <assert.h> // for assert
#include "shm_handler.h" // shm allocator
#include "data_structs.h" // for Stack, BitMap

// TODO move realloc here according to policy
// No sz testing is done here. This is an internal
// structure, no code should use it but friends

const unsigned short MAX_COL_NUM=20;
const unsigned short MAX_ARRAY_LAY_SZ=4096;


typedef mem_db::BufferSz ColName;
typedef mem_db::BufferSz ResultData;
typedef mem_db::Uint32 RawVal;

typedef struct tResultBuff
{
  tResultBuff():sz(0){}
  ResultData resData[MAX_OBJ_NUM];
  unsigned int sz;
  unsigned int indices[MAX_OBJ_NUM];
}ResultBuff;


#define __DAT_TO_RAW(DAT) \
  *(reinterpret_cast<RawVal*>(&DAT))


// should be filled in the srv!!

unsigned int
g_type_ref [6]={sizeof(int),
                sizeof(long),
                sizeof(long long),
                sizeof(float),
                sizeof(double),
                128};

typedef struct tColumnDef
{
  tColumnDef(const ColName& i_name, 
             unsigned int i_loc,
             mem_db::ColTyp i_colTyp): loc(i_loc), 
                                       typsz(g_type_ref[i_colTyp])
  {
    memcpy(name.buff, i_name.buff, i_name.sz);
    name.sz=i_name.sz;
  }
  tColumnDef(){}
  ColName      name;
  unsigned int loc;
  unsigned int typsz;
}ColumnDef;


// resides on shm
typedef struct tLayoutHeader
{
  unsigned int sz; // how many
  ColumnDef columns[MAX_COL_NUM];
  
  inline ColumnDef* get_def_by_name(const ColName* i_pName)
  {
    assert(0 < sz);
    for(unsigned int i=0;i<sz;++i)
      if(i_pName->sz == columns[i].name.sz &&
         0 == memcmp(i_pName->buff, columns[i].name.buff, i_pName->sz))
	return &columns[i];
    return 0;
  }

  inline int addColumnDef(const ColumnDef& i_pDef)
  {
    if(MAX_COL_NUM == sz) return -1; // no more room
    memcpy(columns[sz].name.buff, i_pDef.name.buff, i_pDef.name.sz);
    columns[sz].name.sz=i_pDef.name.sz;
    columns[sz].loc=i_pDef.loc;
    columns[sz++].typsz=i_pDef.typsz;
    return 0;
  }

}LayoutHeader;

// resides on v-memory
typedef struct tComparable
{
  tComparable(ColName* i_names,
              RawVal* i_vals, 
              unsigned int i_sz,
              LayoutHeader* i_pHead)
  {
    assert(0 < i_sz); // used by clients
    assert(MAX_COL_NUM >= i_sz);
    for(unsigned int i=0 ; i<i_sz ; ++i)
    {
      colDef[i]=i_pHead->get_def_by_name(&i_names[i]);
      pVals[i]=i_vals[i];
    }
    sz=i_sz;
  }
  unsigned int sz; // How many
  ColumnDef* colDef[MAX_COL_NUM];
  RawVal pVals[MAX_COL_NUM];
}Comparable;

class ArrayLayout;

/*POD - No code allowed*/
class ArrayLayoutData
{
 private:
  friend class ArrayLayout;
  ArrayLayoutData();//pure+private
  ~ArrayLayoutData();
  BitMap m_bitMap;
  Stack m_stack;
  LayoutHeader m_Header;
  unsigned int dbSz;
};


class LayoutsUT;

class ArrayLayout
{
private:

   // These pointers hold v-memory
  // dont put them on the shm!!!
  Stack* m_vpStack;
  BitMap* m_vpBitMap;
  LayoutHeader* m_vpHeader;
  char* m_vpData;
  unsigned int  m_totTypSz;
  unsigned int* m_vpDbSz;

  // friendship for testing
  friend class LayoutsUT;

  // pure work around - need to implement layoutMngr
  mem_allocator::Pointer m_data;

 public:

  // y is this needed?
  //mem_allocator::Pointer getPoint()const {return m_data;}
  //LayoutHeader* getHead()const{return m_pHeader;}
  
  /* 
  * C'tor - allocates memory here!! 
  */
  ArrayLayout(const LayoutHeader& i_head,
              unsigned int i_sz)
  {
    // init
    void* pshm=0;
    m_totTypSz=0;
    m_vpStack=0;
    m_vpBitMap=0;
    m_vpHeader=0;
    m_vpData=0;
    m_vpDbSz=0;
   
    printf("in layout, allocating sz %u\n", i_sz);

    mem_allocator& alloc= mem_allocator::get_alloc();
    

    printf("sz requested from layout: %u\n",i_sz);
    
  
    assert(0 < (pshm = alloc.allocChunk(i_sz+
					sizeof(ArrayLayoutData),m_data)));
    assert(MAX_COL_NUM > i_head.sz);
    // assign pointers
    m_vpHeader= __RE_INTPRT_SHM_TO_TYPE(LayoutHeader, pshm);
    m_vpStack= __RE_INTPRT_SHM_TO_TYPE(Stack, 
                                     m_vpHeader+1);
    for(unsigned int i=0 ; i<MAX_OBJ_NUM ; i++)
      m_vpStack->push(i);

     assert(m_vpStack->getCapacity() == m_vpStack->getSz());

    m_vpStack->init(MAX_OBJ_NUM);
    m_vpBitMap= __RE_INTPRT_SHM_TO_TYPE(BitMap, 
                                     m_vpStack+1);
    m_vpBitMap->init();
    
    m_vpDbSz= __RE_INTPRT_SHM_TO_TYPE(unsigned int, 
				      m_vpBitMap+1);
  
    m_vpData=__RE_INTPRT_SHM_TO_TYPE(char, 
                                     m_vpDbSz+1);

    for(unsigned int i=0 ; i<i_head.sz ;++i)
    {
      memcpy(m_vpHeader->columns[i].name.buff,
             i_head.columns[i].name.buff,
             i_head.columns[i].name.sz);
      m_vpHeader->columns[i].loc=i_head.columns[i].loc;
      m_vpHeader->columns[i].name.sz=i_head.columns[i].name.sz;
      m_vpHeader->columns[i].typsz=i_head.columns[i].typsz;
      m_totTypSz+=i_head.columns[i].typsz; // stupid!!
       printf("added column name: %.*s\n", 
           m_vpHeader->columns[i].name.sz ,
           m_vpHeader->columns[i].name.buff);
    }
    m_vpHeader->sz=i_head.sz;
    printf("finished registering tot type sz: %u\n", m_totTypSz);
    
    __PRINT_ADDR(pshm)

      

#ifdef __HAS_SHM_RPRT_ 
    mem_allocator::updateActSz(m_totTypSz, pshm);
#endif //  __HAS_SHM_RPRT_ 
    
    // TODO register the layout in the layout registry
    // use the data local variable
  }

  /*
  * This gets a virtual pointer to
  * data in virtual memory, and creats
  * a shared memory entry in the layout
  */
  int addEntry(void* i_pDat)
  {
    assert(0 < i_pDat);
    assert(0 < m_vpStack->getSz());
  
    if(0 == m_vpBitMap->bits) return -1;

    char* vpdata = (char*)m_vpData;
    char* vpinp = __RE_INTPRT_SHM_TO_TYPE(char, i_pDat);

    unsigned int val= m_vpStack->pop();
    m_vpBitMap->bit_on(val);
    printf("val poped %u\n", val);

    vpdata+=(val*m_totTypSz);
   
    // Writing data
    unsigned int copied=0;
    for(unsigned int i=0;i<m_vpHeader->sz;++i)
    {
      memcpy(vpdata+copied, vpinp+copied, m_vpHeader->columns[i].typsz);
      copied+=m_vpHeader->columns[i].typsz;
    }
    
    // TODO fix it so the alloc mngr knows alloc was made
    //mem_allocator& alloc= mem_allocator::get_alloc();
     //alloc.updateActSz(copied, m_pHeader);
    return 0;
  }

  unsigned int updateEntry(const Comparable& i_selecComp,
                           const Comparable& i_updComp)
  {
    assert(0 < m_vpStack->getSz());

    if(0 == m_vpBitMap) return -1; //nothing to update 
    
    ResultBuff result;

    if(0 == findEntry(i_selecComp,
		      &result))
       return 0; // no such obj

   char* vpdata = (char*)m_vpData;
   unsigned int upd=0; 

    for(unsigned int i=0;i<result.sz;++i)
    {
      vpdata+= (result.indices[i]*m_totTypSz);
      for(unsigned int j=0;j<i_updComp.sz;++j)
      {
	vpdata+=i_updComp.colDef[j]->loc;
	memcpy(vpdata, &i_updComp.pVals[j], i_updComp.colDef[j]->typsz);
	upd++;
       }
     } 
    return upd;
  }

  /*
  * This simply marks the bits as available again
  * The data sent contains a field that is the
  * comparable which is used to locate the item
  */
  int delEntry(const Comparable& i_comp)
  {
    assert(0 < m_vpStack->getSz());

    if(0 == m_vpBitMap->bits)
       return 0; //empty == no such obj
   
    ResultBuff result;
    
    unsigned int indx=0;
    if(0 == (indx = findEntry(i_comp,
                              &result))) 
        return 0; // no such obj

    for(unsigned int i=0;i<result.sz;++i)
    {
      // TODO check return sts
       m_vpStack->push(result.indices[i]);
       m_vpBitMap->bit_off(result.indices[i]);
    }
    return result.sz;
  }


  /*
  * 
  */
  unsigned int findEntry(const Comparable& i_comp, 
                         ResultBuff* o_pRes)
  {

    assert(0 < m_vpStack->getSz());

    unsigned short matchesFound=0, totMatchFound=0;

    // Ok in an ideal world, there would be a heap
    // holding the max bit num in the map. For now
    // to prevent adding an 'if' in the bmap, just
    // go over all the bits, and check
    
    for(unsigned int objIndx=0; objIndx<UINT_MAX; ++objIndx)
    {
      if(false == m_vpBitMap->is_on(objIndx)) continue;
      printf("looking into obj %u\n", objIndx);
      char* vpdata = (char*)m_vpData;
      vpdata+=(objIndx*m_totTypSz);
      for(unsigned int compIndx=0;
	  compIndx<i_comp.sz;
	  compIndx++)
	{
          ColumnDef* pcolDef =i_comp.colDef[compIndx];
          RawVal compVal=i_comp.pVals[compIndx];
         
	  if(0 == memcmp(&compVal, vpdata+(pcolDef->loc),pcolDef->typsz))
	  {
	     // FOUND A MATCH
	     matchesFound++;
             if(matchesFound == i_comp.sz)
	     {
	       printf("obj %u matches nicely\n", objIndx);
               totMatchFound++;
               matchesFound=0;
               break;
	      }
	   }
	} // for compIndx
    } // for objIndx
    return totMatchFound;
  }

  /*Cleans shm!!*/
  int destroy()
  {
    // TODO take shm pointer from registry
    /*mem_allocator& alloc= mem_allocator::get_alloc();
      __SYS_CALL_TEST_RETURN(alloc.disposeChunk(m_data));*/
    return 0;
  }

  /*
  * This function is used to index the
  * array. Anyone wishing to implement it
  * needs to comply to this API. The return
  * value has to be a valid index num 
  * TODO - is a 64 bit num enough? [-1 reserved for error]
  */
  //int (*pHashFunc) (void* TYP); 
};




#endif // __MEM_DB_SERVER_LAYOUTS_INC_LAYOUTS_H_
