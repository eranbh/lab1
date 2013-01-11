#ifndef __MEM_DB_COMMON_INC_NETWORK_INTERFACE_H_
#define __MEM_DB_COMMON_INC_NETWORK_INTERFACE_H_
#include "types.h"
/*
* Handles communication issues
*/
const unsigned short DEF_IN_PORT=8888;
const unsigned short DEF_OUT_PORT=8889;


/*
* This class implements a basic TCP
* acceptor [listening] that can be set
* to accept data. The use of its APIs
* may block, so use with caution
*/
class BlockingTcpAcceptor
{
 public:
  // TODO - take from page sz
  static const unsigned int DEFAULT_BUFF_SZ=4096;
  // Builds a listening socket
  // Buff sz - default amount to accept
  BlockingTcpAcceptor(unsigned short i_port=DEF_IN_PORT, 
                      unsigned int i_buffSz=DEFAULT_BUFF_SZ);
  ~BlockingTcpAcceptor(){}

  // This is where the epoll is done
  // This may block when there are no
  // events.
  int listen2Events(mem_db::NetworkMsg& o_msg);

 private:


// Reads up to 4096 [page_sz] bytes
// This means NO query should exceed that
// Reads in the following order:
// 1. Query header
// 2. Query body
// 3. 1 byte - indicating more queries
  int readData(int i_fs, unsigned int i_msgSz, char* i_pBuff);


  // The socket on which accept is done
  int m_sockFD;

  unsigned int m_buffSz;
};


// Represents the header for
// the query sent over the net
// This is NOT a business representation
// of the query, but the network one
typedef struct tComQueryHeader
{
  tComQueryHeader():
     m_hasMore(0),
     m_session(0),
     m_messageSz(0){}


  unsigned short  m_hasMore;
  unsigned short  m_session;
  unsigned int    m_messageSz;
}ComQueryHeader;

static const unsigned short 
COMM_HEAD_SZ=sizeof(ComQueryHeader);

////////////////////////////////////////////////////////

class BlockingTcpTransmitor
{
 public:
  BlockingTcpTransmitor(unsigned short    i_port=DEF_OUT_PORT,
                         const char* const i_pHostNm="localhost");
  ~BlockingTcpTransmitor(){}

  // Sends the buffer over the established conn
  // May block !!!!
  int writeData(unsigned int i_sz, char* i_pBuff);

 private:
  // Socket on which transmission is done
  int m_sockFD;
  
};

#endif // __MEM_DB_COMMON_INC_NETWORK_INTERFACE_H_
