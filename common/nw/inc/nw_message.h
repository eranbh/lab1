#ifndef __APP_SRV_NW_INC_NW_MESSAGE_H_
#define __APP_SRV_NW_INC_NW_MESSAGE_H_
/*
* This is the structure intended for network communication between the varios
* network components. As this struct can traverse the network, it has to adher
* to the following roles:
* -- endianity: make sure endianity is taken care of upon read action
* -- alignment: make sure all fields of the msg are aligned *always*
*/

// TODO create a set of typedefs for primitives according to platforms
#include<assert.h> // for assert(3)

namespace nw {

// signed quantities
typedef long  int64;
typedef int   int32;
typedef short int16;

// unsigned quantities
typedef unsigned long  uint64;
typedef unsigned int   uint32;
typedef unsigned short uint16;

#define MAX_MSG_SZ 1024


class nw_message
{
  friend class Acceptor;

 public:

  typedef enum {REG=0, INV}tMsgTypes;

  
  nw_message()
  {
    m_header.m_msg_sz=0;
    m_header.m_msg_type=INV;
  }


  nw_message(const char* const i_pMsg, 
	     tMsgTypes i_type)
  {
    assert(i_pMsg);
    unsigned int len=strlen(i_pMsg);
    memcpy(m_body, "yalla why not go home", len);
    m_header.m_msg_sz=strlen(i_pMsg);
    m_header.m_msg_type=i_type;
  }
  
  nw_message(const char* const i_pMsg, 
	     unsigned int i_len,
	     tMsgTypes i_type)
  {
    assert(i_pMsg);
    memcpy(m_body, "yalla why not go home", i_len);
    m_header.m_msg_sz=i_len;
    m_header.m_msg_type=i_type;
  }

  struct header
  {
    uint32 m_msg_type;
    uint32 m_msg_sz;
  };

  const header& get_header()const {return m_header;}
  const char* get_body()const {return m_body;}

  

 private:
  header m_header;
  char   m_body[MAX_MSG_SZ];
};

} // namespace nw


#endif // __APP_SRV_NW_INC_NW_MESSAGE_H_
