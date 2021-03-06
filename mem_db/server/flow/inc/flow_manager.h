/*
* Holds the flow manager definition
*/
#include "Task.h"
#include "shm_handler.h"
#include "network_interface.h"
#include "layouts.h"

const unsigned short MAX_TBL_NM=256;
#define BAD_TB_NM ((unsigned short)-1)

template<typename TYP>
struct TblNm2DataBuff
{
  void init(){indx=0;}
  mem_db::BufferSz tbNm[MAX_TBL_NM];
  
  int findTbIdx(char nm[], unsigned short sz)const
  {
    for(unsigned int i=0;i<=indx;++i)
    {
      if(sz != tbNm[i].sz) continue;
      if(0 == memcmp(nm,tbNm[i].buff, sz))
	return i;
    }
    return -1;
  }
  
  unsigned int tblNmSz;
  TYP data[MAX_TBL_NM];
  unsigned short indx;
};

class FlowMngr : public MultiCore_Task
{
 public:
  FlowMngr();
  ~FlowMngr(){}
 protected:
  virtual int init();
  virtual int run();
  virtual int fini();

 private:
  BlockingTcpAcceptor m_acceptor;
  //BlockingTcpTransmitor m_transmitor;
  mem_db::NetworkMsg inMsg;
  mem_db::NetworkMsg outMsg;
  typedef struct TblNm2DataBuff<mem_allocator::Pointer> TblNm2ShmPnt;
  typedef struct TblNm2DataBuff<ArrayLayout*> TblNm2ArrLay;
  TblNm2ShmPnt* m_tbNm2Pt; // this is bad!!!
  TblNm2ArrLay* m_tbNm2ArrLay; // bad!!
  int handle_create(mem_db::NetworkMsg& i_msg);
  int handle_insert(mem_db::NetworkMsg& i_msg);
  int handle_update(mem_db::NetworkMsg& i_msg){return 0;}
  int handle_select(mem_db::NetworkMsg& i_msg);
  unsigned int getTypSzByStr(char tyNm[],
                             unsigned short sz,
                             mem_db::ColTyp& o_typ);
  void showObj(LayoutHeader&,
               char* buff);
  void showLayout(char layNm[], 
                  unsigned int layNmSz,
                  LayoutHeader& i_head);

};
