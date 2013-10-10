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

} // namespace fw

#endif // __COMMON_SHR_TYPES_H
