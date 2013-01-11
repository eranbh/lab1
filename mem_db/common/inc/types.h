#ifndef  __MEM_DB_COMMON_TYPES_H_
#define  __MEM_DB_COMMON_TYPES_H_
/**/

namespace mem_db{

  // LINUX 32BIT!!!!
  typedef unsigned short Uint16;  
  typedef unsigned int   Uint32;
  typedef unsigned long long Uint64;
  const unsigned short MAX_BUFF_SZ=512;

  typedef struct tBufferSz
  {
	unsigned int sz;
 	char buff[128];	
  }BufferSz;

  typedef enum {CR=1,INST,UPD,SEL} QueryTyp;
  typedef enum {INT=0,LONG,LLONG,FLOAT,DOUBLE,VCR} ColTyp;

  const unsigned short MAX_NET_MSG=4096;

  typedef struct tNetworkMsg
  {
    char buff[MAX_NET_MSG]; // pg sz?
    unsigned int layNmSz;
    unsigned int paramSz;
    QueryTyp qTyp;
  
  }NetworkMsg;

} // mem_db

#endif // __MEM_DB_COMMON_TYPES_H_
