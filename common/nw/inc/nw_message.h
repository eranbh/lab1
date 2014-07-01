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
#include <string.h>


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

class AcceptorBaseMessages
{
 public:
  typedef enum {REG=0, TRM, INV}tMsgTypes;
  typedef tMsgTypes this_type;
};


template<typename MSG_TYPS=AcceptorBaseMessages>
class Iheader
{
 public:
 
 
  typedef MSG_TYPS msg_types;
  typedef typename MSG_TYPS::this_type this_type;

 uint32 getMsgSz()const{return m_msg_sz;}
 this_type getMsgType()const{return m_msg_type;}
 void setMsgSz(uint32 a_msg_sz){m_msg_sz=a_msg_sz;}
 void setMsgType(this_type a_msg_type){m_msg_type=a_msg_type;}
 
  virtual void init()
  {    
      m_msg_sz=0;
      m_msg_type=msg_types::INV;
  }

protected:

  ~Iheader(){}
   Iheader(){}
 
  uint32 m_msg_sz;
  this_type m_msg_type;
private: // never allow access to this !!!
  Iheader(const Iheader&);
  Iheader& operator=(const Iheader&);
};

struct header : public Iheader<>
{
 
  // MANY other fields that can go here ... 
#ifdef __TESTING_MODE
  uint32 m_msgSeq; // should be used to map msg's to results
#endif // __TESTING_MODE
  const header& operator=(const header& a_other)
     {return *this;}

void init()
    {    
#ifdef __TESTING_MODE
      m_msgSeq = 0;
#endif // #ifdef __TESTING_MODE
    }
};


// typedef enum {REG=0, TRM, INV}tMsgTypes;
template<typename HEADER=header>
class nw_message
{
  friend class Acceptor;

 public:

  typedef typename HEADER::msg_types msg_types;
  typedef typename HEADER::this_type this_type;

  nw_message()
  {
    m_header.init();
  }

  void init(const char* const i_pMsg, 
	    unsigned int i_len,
	    this_type i_type)
  {
    memcpy(m_body, i_pMsg, i_len);
    m_header.setMsgSz(i_len);
    m_header.setMsgType(i_type);
  }

 
  nw_message(const char* const i_pMsg, 
	     unsigned int i_len,
	     this_type i_type)
  {
    assert(i_pMsg);
    init(i_pMsg, i_len, i_type);
  }

 

  HEADER& get_header() {return m_header;}
  const char* get_body()const {return m_body;}
  void set_msgTyp(this_type i_type){m_header.setMsgType(i_type);}

  

 private:
  HEADER m_header;
  char   m_body[MAX_MSG_SZ];
};

} // namespace nw


#endif // __APP_SRV_NW_INC_NW_MESSAGE_H_
