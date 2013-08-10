/*
* The purpose of this class is to handle the stream of data between
* a SocketServer and a SocketClient with efficiancy.
*/


namespace JethroData{
  namespace DataStreamManager{



class AbstractDataStreamMngr
{
 public:
 AbstractDataStreamMngr(TCPSocket* i_socket):m_socket(i_socket)
  {}

  virtual init() {/*perform some default init*/}

  virtual reset {/* perform some default reset */}


  /* common members */
 private:
  TcpSocket* m_socket;
}


// Interfaces

class DataInStreamMngr : public AbstractDataStreamMngr 
{
  virtual int handleProtoBuffRecv() = 0;
} 


class DataOutStreamMngr : public AbstractDataStreamMngr
{
  virtual int handleProtoBuffSend() = 0;
} 


class BulkDataInStreanMngr : public DataInStreamMngr
{
 public:

  /*
  * C'tor
  * Data that is built once per socket
  */
  BulkDataInStreamMngr(TCPSocket * i_socket):m_socket(i_socket)
  {
    assert(m_socket);
  }

  /*Inits data that is built per request */
 void init(char* i_recvBuff)
  {
  }
  

  /*                                                          
  * main entry point for the receive logic                                      
  * TODO add algorithm
  */
  handleProtoBuffRecv();

  /*reseting state*/
  reset();



 private:

  handleFinalizeReceive();

  
};


class BulkDataOutStreamMngr : public DataOutStreamMngr
{

 public:

   /*
  * C'tor
  * Data that is built once per socket
  */
 BulkDataOutStreamMngr(TCPSocket * i_socket):AbstractDataStreamMngr(i_socket)
  {
    assert(m_socket);
  }

  /*Inits data that is built per request */
  void init(char* i_recvBuff)
  {
    
  }

  void reset();
  
  /*
  * main entry point for the send logic
  * TODO add algorithm
  */
  handleProtoBuffSend();

 private:
}


} // namespace JethroData
