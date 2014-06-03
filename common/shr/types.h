#ifndef __COMMON_SHR_TYPES_H
#define __COMMON_SHR_TYPES_H
#include <assert.h>

namespace fw{ 

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


        /* no copy c'tor */
		tBufferSz(const tBufferSz&){}

		/* no operator=  */
		const tBufferSz& operator=(const tBufferSz&){return *this;}

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
template<typename T>
class GenContainer
{
 public:
  /*need to access my privates when copying*/
  friend class GenContainer;

 GenContainer(const GenContainer& a_other):
  m_sz(a_count.m_sz)
 {
   assert(a_other.m_sz && a_other.m_datum);

   for(size_t i=0;i<a_sz;++i)
   {
     // it is assumed that the obj is copy'able
     m_datum[i] = new T(a_other.m_datum[i]);
   }
 }
  
private:
  unsigned int m_sz;
  T* m_datum;
};


} // namespace fw

#endif // __COMMON_SHR_TYPES_H
