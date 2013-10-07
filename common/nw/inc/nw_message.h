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

  typedef enum {REG=0, TRM, INV}tMsgTypes;

  
  nw_message()
  {
    m_header.m_msg_sz=0;
    m_header.m_msg_type=INV;
    m_header.m_msgSeq = 0;
  }

  void init(const char* const i_pMsg, 
	     unsigned int i_len,
	     tMsgTypes i_type)
  {
    memcpy(m_body, i_pMsg, i_len);
    m_header.m_msg_sz=i_len;
    m_header.m_msg_type=i_type;
  }


  nw_message(const char* const i_pMsg, 
	     tMsgTypes i_type)
  {
    assert(i_pMsg);
    unsigned int len=strlen(i_pMsg);
    assert(len);
    init(i_pMsg, len, i_type);
   
  }
  
  nw_message(const char* const i_pMsg, 
	     unsigned int i_len,
	     tMsgTypes i_type)
  {
    assert(i_pMsg);
    init(i_pMsg, i_len, i_type);
  }

  struct header
  {
	  uint32 m_msg_sz;
#ifdef __TESTING_MODE
	  uint32 m_msgSeq; // should be used to map msg's to results
#endif // __TESTING_MODE
	  tMsgTypes m_msg_type;
  };

  header& get_header() {return m_header;}
  const char* get_body()const {return m_body;}
  void set_msgTyp(tMsgTypes i_type){m_header.m_msg_sz=i_type;}

  

 private:
  header m_header;
  char   m_body[MAX_MSG_SZ];
};

} // namespace nw


#endif // __APP_SRV_NW_INC_NW_MESSAGE_H_
