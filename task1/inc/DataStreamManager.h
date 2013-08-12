/*
* The purpose of this class is to handle the stream of data between
* a SocketServer and a SocketClient with efficiancy.
*/


namespace JethroData{
  namespace DataStreamManager{


class JethroMessage
{};

class JethroDataMessage::Request : public  JethroMessage
{};

 class JethroDataMessage::Respond : public JethroMessage
{};

const UDWordType MAX_TRX_BUFFER_SZ= 2 * 0x100000; // 2Mb. should be from conf

/*
* Interface for arg storage
*/
typedef struct tDataStreamArgs
{}DataStreamArgs;

 
typedef struct tDataInStreamArgs : public DataStreamArgs 
{}DataInStreamArgs;


typedef struct tDataOutStreamArgs : DataStreamArgs
{
  
 tDataOutStreamArgs(const std::string& i_query):
                                m_query(i_query){}
  /**/
  const std::string& m_query; 
}DataOutStreamArgs;



class AbstractDataStreamMngr
{
 public:
 AbstractDataStreamMngr(TCPSocket* i_socket):m_socket(i_socket)
  {}

  virtual void init(DataStreamArgs* i_args) 
  {
    /*perform some default init*/
    m_args = i_args;
  }

  virtual void reset() {/* perform some default reset */}

  virtual int finalizeStream(JethroMessage&) {/*perform some default cleanup*/}


  /* common members */
 private:
  TcpSocket* m_socket;

  DataStreamArgs* m_args;
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
  BulkDataInStreamMngr(TCPSocket * i_socket);

  /*Inits data that is built per request */
  virtual void init(DataStreamArgs*);
  
  /*                                                          
  * main entry point for the receive logic                                      
  * TODO add algorithm
  */
  virtual int handleProtoBuffRecv();

 private:

  int recvBulk();

  /*reseting state*/
  virtual void reset();

  virtual int finalizeStream(JethroMessage&);
  
};


class BulkDataOutStreamMngr : public DataOutStreamMngr
{

 public:

   /*
  * C'tor
  * Data that is built once per socket
  */
  BulkDataOutStreamMngr(TCPSocket * i_socket);

  /*Inits data that is built per request */
  virtual void init(DataStreamArgs*);

  /*
  * main entry point for the send logic
  * TODO add algorithm
  */
  virtual int handleProtoBuffSend();

 private:

  int sendBulk();
  
  virtual void reset();

  virtual int finalizeStream(JethroMessage&);
};


} // namespace JethroData
