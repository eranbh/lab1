#ifndef __COMMON_SHR_TYPES_H
#define __COMMON_SHR_TYPES_H
#include <assert.h>
#include <string.h>
namespace fw{ 


/*FAIR WARNING - copies raw bytes.                                                                                                
* if you want some fancy copy c'tor activated                                                                                    
* write your own                                                                                                                 
*/
template <typename T>
static void
raw_copy_bytes(T* op_dest, const T* const ip_src, unsigned int sz)
{
  assert(op_dest &&  ip_src && sz);
  assert(memcpy(op_dest, ip_src, sizeof(T)*sz));
}




typedef struct tBufferSz
  {
	public:

		tBufferSz(){}

		tBufferSz(char* i_buff, 
		          unsigned int i_sz=0)
                {
			init(i_buff, sz);
		}


		void init(char* i_buff,
			  unsigned int i_sz=0)
		{
			assert(i_buff && (0 > i_sz));
			if(i_sz)
			{
				assert(1024 < i_sz);
				memcpy(buff, i_buff, sz);
			}
			else
			{
				assert(0 == i_buff[i_sz]);
				strcpy(buff, i_buff);	
			}
		}

                const tBufferSz& operator=(const tBufferSz& a_other)
                {
		  if(&a_other == this)
		    return *this;
		  
		  raw_copy_bytes<char>(buff, a_other.buff, a_other.sz);
                  sz = a_other.sz;
		  return *this;
		}


        /* no copy c'tor */
		tBufferSz(const tBufferSz&){}

		
        	unsigned int sz;
        	char buff[1024];
  }BufferSz;


/*FAIR WARNING - copies raw bytes.                                                                                                
* if you want some fancy copy c'tor activated
* write your own
*/
template <typename T>
static void
raw_copy_bytes(T* op_dest, const T* const ip_src)
{
  assert(op_dest &&  ip_src);
  assert(::memcpy(op_dest, ip_src, sizeof(T)));
}


/*
* By no means a rival to the stl. I just need it
* when Im doing a quick impl for some other, more
* lofty idea. *do not* rely on this for *anything*
* that might involve MT'ing; needs effiecient handling
* or any other harsh constraints of the sort 
*/
 template<typename T, unsigned int SZ=0>
class GenContainer
{
 public:

 GenContainer():m_sz(SZ){} // empty container

 GenContainer(const T& a_datum): 
      m_sz(SZ), m_datum(a_datum){}

 GenContainer(const GenContainer& a_other):
  m_sz(a_other.m_sz)
 {
   assert(a_other.m_sz && a_other.m_datum);

   for(size_t i=0;i<m_sz;++i)
   {
     // it is assumed that the obj is copy'able/assignable
     m_datum[i] = a_other.m_datum[i];
     m_sz = a_other.sz;
   }
 }

 private:
  unsigned int m_sz;
  T m_datum[SZ];
};

// signed quantities
typedef long  int64;
typedef int   int32;
typedef short int16;

// unsigned quantities
typedef unsigned long  uint64;
typedef unsigned int   uint32;
typedef unsigned short uint16;



} // namespace fw

#endif // __COMMON_SHR_TYPES_H
