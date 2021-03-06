/*
* Implements the main flow task
*/
#include "macros.h"
#include "flow_manager.h"
FlowMngr::FlowMngr()
{
  
}

int FlowMngr::init()
{
  return 0;
}
int FlowMngr::run()
{
  while(1)
  {
    __SYS_CALL_TEST_RETURN(m_acceptor.listen2Events(inMsg))
      printf("query typ: %u\n",inMsg.qTyp);
      printf("param sz: %u\n",inMsg.paramSz);
      printf("laynm %.*s\n",inMsg.layNmSz,inMsg.buff);
      switch(inMsg.qTyp)
      {
        case mem_db::CR:
	{
	  handle_create(inMsg);break;
        }
        case mem_db::INST:
	{
	  handle_insert(inMsg);break;
        }
        case mem_db::UPD:
	{
	  handle_update(inMsg);break;
        }
        case mem_db::SEL:
	{
	  handle_select(inMsg);break;
        }

      }
    
    //break;
    }
  return 0;
}

int FlowMngr::fini()
{
  return 0;
}


int FlowMngr::
handle_create(mem_db::NetworkMsg& i_msg)
{
  //mem_allocator& alloc = mem_allocator::get_alloc();
  //mem_allocator::Pointer point;
  //void* pshm=0;
  // point=2;
  //TblNm2ShmPnt* nm2Pointer;
  // if(0 == (pshm=alloc.pointerToLocalPointer(point)))
  // { // first time alloc the reg
  // pshm=alloc.allocChunk(sizeof(TblNm2ShmPnt),point);
  m_tbNm2ArrLay = new TblNm2ArrLay;
      // reinterpret_cast<TblNm2ShmPnt*>(pshm);
  m_tbNm2ArrLay->init();
  m_tbNm2Pt = new TblNm2ShmPnt;
      // reinterpret_cast<TblNm2ShmPnt*>(pshm);
   m_tbNm2Pt->init();
     //}
 
  // assert(2 == point); // even alloc should return 2!!
  //assert(0 < pshm);
  
  // create layout header
  unsigned int accStrSz=0, perStrSz=0;
  mem_db::ColTyp type;
  LayoutHeader header;
  char* pmsg=i_msg.buff;
  pmsg+=i_msg.layNmSz; // skip laynam
  unsigned int colDefSz=0;

  // reading tuple <col name,type>
  while(accStrSz < i_msg.paramSz)
  {
    perStrSz=*(reinterpret_cast<unsigned int*>(pmsg));
    if(0 >= perStrSz) return -1;
    pmsg+=sizeof(unsigned int);
    accStrSz+=sizeof(unsigned int);
    memcpy(header.columns[colDefSz].name.buff, pmsg, perStrSz);
    header.columns[colDefSz].name.sz=perStrSz;
    pmsg+=perStrSz;
    accStrSz+=perStrSz; 
    perStrSz=*(reinterpret_cast<unsigned int*>(pmsg));
    if(0 >= perStrSz) return -1;
    pmsg+=sizeof(unsigned int);
    accStrSz+=sizeof(unsigned int);
    header.columns[colDefSz].typsz=getTypSzByStr(pmsg, perStrSz, type);
    if((unsigned int)-1 ==  header.columns[colDefSz].typsz)return -1;
    header.columns[colDefSz].typ=type;
    pmsg+=perStrSz;
    accStrSz+=perStrSz;
    colDefSz++;
  }
  if(MAX_COL_NUM < colDefSz) return -1;//tbl 2 small
  header.sz=colDefSz;

  // register
  //m_tbNm2Pt = 
  //reinterpret_cast<TblNm2ShmPnt*>(pshm);

  // reg layout
  ArrayLayout* lay = new ArrayLayout(header, MAX_ARRAY_LAY_SZ);
  memcpy(m_tbNm2Pt->tbNm[m_tbNm2Pt->indx].buff, i_msg.buff, i_msg.layNmSz);
  m_tbNm2Pt->tbNm[m_tbNm2Pt->indx].sz=i_msg.layNmSz;
  m_tbNm2Pt->data[m_tbNm2Pt->indx++]=lay->getPoint();

  memcpy(m_tbNm2ArrLay->tbNm[m_tbNm2ArrLay->indx].buff,
            i_msg.buff, i_msg.layNmSz);
  m_tbNm2ArrLay->tbNm[m_tbNm2ArrLay->indx].sz=i_msg.layNmSz;
  m_tbNm2ArrLay->data[m_tbNm2ArrLay->indx++]=lay;

  printf("created layout %.*s\n", i_msg.layNmSz, i_msg.buff);

  return 0;
}

unsigned int FlowMngr::
getTypSzByStr(char tyNm[], unsigned short sz, mem_db::ColTyp& o_typ)
{
  if(0 == memcmp(tyNm,"int",sz))
  {
    o_typ=mem_db::INT;
    return sizeof(int);
  }
  else if(0 == memcmp(tyNm,"float",sz))
    {
      o_typ=mem_db::FLOAT;
     return sizeof(float);
    }
  else if(0 == memcmp(tyNm,"long",sz))
    {
       o_typ=mem_db::LONG;
     return sizeof(long);
    }
  else if(0 == memcmp(tyNm,"double",sz))
    {
       o_typ=mem_db::DOUBLE;
     return sizeof(double);
    }
  else if(0 == memcmp(tyNm,"long long",sz)) // just lnx!!!
    {
      o_typ=mem_db::LLONG;
     return sizeof(long long);
    }
  else if(0 == memcmp(tyNm,"varc",sz)) // up 2 32b :)
    {
      o_typ=mem_db::VCR;
     return 4;
    }
  else return -1;
}


const unsigned short MAX_RES_SZ=50;

int FlowMngr::
handle_select(mem_db::NetworkMsg& i_msg)
{
  // void* pshm=0;
  //mem_allocator& alloc = mem_allocator::get_alloc();
  //xsmem_allocator::Pointer point=2;
  //assert(0 < (pshm=alloc.pointerToLocalPointer(point)));

  //TblNm2ShmPnt* nm2Pointer = 
  // reinterpret_cast<TblNm2ShmPnt*>(pshm);

  // First locate the layout
  unsigned short idx=0;
  ArrayLayout* lay=0;
  if(-1 ==
     (idx= m_tbNm2ArrLay->findTbIdx(i_msg.buff, i_msg.layNmSz)))
  {
    printf("No Such Obj\n");
    return -1;
  }
  
  lay=m_tbNm2ArrLay->data[idx];
  //pshm= 
  //  alloc.pointerToLocalPointer(m->data[idx]);
  // if(0 == pshm) return -1;

  // ArrayLayout play
  // build Comparable
  ColName cols[MAX_COL_NUM];
  RawVal vals[MAX_COL_NUM];

  idx=0;
  unsigned int accStrSz=0, perStrSz=0;
  char* pmsg=i_msg.buff;
  pmsg+=i_msg.layNmSz; // skip laynam

  // reading tuple <colNm,val>
  while(accStrSz < i_msg.paramSz)
  {
    perStrSz=*(reinterpret_cast<unsigned int*>(pmsg));
    if(0 >= perStrSz) return -1;
    pmsg+=sizeof(unsigned int);
    accStrSz+=sizeof(unsigned int);
    memcpy(cols[idx].buff, pmsg, perStrSz);
    cols[idx].sz=perStrSz;
    pmsg+=perStrSz;
    accStrSz+=perStrSz;
    RawVal val = *(reinterpret_cast<RawVal*>(pmsg));
    memcpy(&vals[idx++], &val, sizeof(RawVal));
    pmsg+=sizeof(RawVal);
    accStrSz+=sizeof(RawVal);
  }
  Comparable comp(cols,vals,idx);
  
  ResultBuff res[MAX_RES_SZ];
  unsigned int resNm=0;
  
  if(0 == (resNm = lay->findEntry(comp, res, resNm)))
  {
    printf("No Obj Selected\n");
    return -2; // no res found
  }
  // found obj[s]
  showLayout(i_msg.buff, i_msg.layNmSz, *lay->getHead());

  for(unsigned int i=0;i<resNm;++i)
    showObj(*lay->getHead/(), res[i].buff);
  
  return 0;
}


void FlowMngr::showLayout(char layNm[], 
                  unsigned int layNmSz,
                  LayoutHeader& i_head)
{
  printf("layout name: %.*s:\n\n", layNmSz, layNm);
  printf("layout definition:\n");

  for(unsigned int i=0;i<i_head.sz;++i)
  {
    printf("column %u name: %.*s  location: %u  raw_sz: %u\n",
            i+1,
            i_head.columns[i].name.sz,
	    i_head.columns[i].name.buff,
            i_head.columns[i].loc,
            i_head.columns[i].typsz);
  }
}


void  FlowMngr::showObj(LayoutHeader& i_head,
                        char* buff)
{ 
  printf("layout data\n");
    for(unsigned int i=0;i<i_head.sz;++i)
    {
      if(mem_db::INT == i_head.columns[i].typ)
	{
	printf("%d\t", *(reinterpret_cast<int*>(buff)));
        buff+=sizeof(int);
	}
      else  if(mem_db::FLOAT == i_head.columns[i].typ)
	{
	printf("%f\t", *(reinterpret_cast<float*>(buff)));
        buff+=sizeof(float);
	  }
      else  if(mem_db::LONG == i_head.columns[i].typ)
	{
	printf("%ld\t", *(reinterpret_cast<long*>(buff)));
        buff+=sizeof(long);
	}
      else  if(mem_db::LLONG == i_head.columns[i].typ)
	{
	printf("%lld\t", *(reinterpret_cast<long long*>(buff)));
        buff+=sizeof(long long);
	  }
      else  if(mem_db::DOUBLE == i_head.columns[i].typ)
	{
	printf("%lf\t", *(reinterpret_cast<double*>(buff)));
        buff+=sizeof(double);
	}
      else  if(mem_db::VCR == i_head.columns[i].typ)
	printf(" %.*s\t", 4,reinterpret_cast<char*>(buff));
    }
   
}

int  FlowMngr::
handle_insert(mem_db::NetworkMsg& i_msg)
{
   // First locate the layout
  unsigned short idx=0;
  ArrayLayout* lay=0;
  if(-1 ==
     (idx= m_tbNm2ArrLay->findTbIdx(i_msg.buff, i_msg.layNmSz)))
  {
    printf("No Such Obj\n");
    return -1;
  }
  
  lay=m_tbNm2ArrLay->data[idx];

  // read the message and build query
  char buff[mem_db::MAX_NET_MSG];
  memset(buff, 0, i_msg.paramSz); // clean mem - not all columns affected
  
  char* pmsg=i_msg.buff+i_msg.layNmSz;
  unsigned int accStrSz=0, perStrSz=0;
  char colName[MAX_COL_NUM];
  LayoutHeader* phead=lay->getHead();
  unsigned int offset=0;

  while(accStrSz < i_msg.paramSz)
  {
    perStrSz=*(reinterpret_cast<unsigned int*>(pmsg));
    if(0 >= perStrSz) return -1;
    pmsg+=sizeof(unsigned int);
    accStrSz+=sizeof(unsigned int);
    memcpy(colName , pmsg, perStrSz);
    pmsg+=perStrSz;
    accStrSz+=perStrSz;
    for(unsigned int i=0;i<phead->sz;i++)
    {
      if(phead->columns[i].name.sz == perStrSz)
	{
	  if(0 == (memcmp(phead->columns[i].name.buff,
			  colName,
			  perStrSz) ))
	    {
	      // Found a match
	      memcpy(buff+offset, pmsg, phead->columns[i].typsz);
              pmsg+=phead->columns[i].typsz;
              accStrSz+=phead->columns[i].typsz;
	      offset=0;
              break;
	    }
	  offset+=phead->columns[i].typsz;
	}
    }//for
    if(0 == offset) return -1; //bad col name
  }//while

  showLayout(i_msg.buff, 4, *phead);
  showObj(*phead, buff);
  return 0;
}
