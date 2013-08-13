/*
* The purpose of this module is to hold classes that implement the data
* stream strategy for the SocketClient/Server classes. 
*/


namespace JethroData{
  namespace DataStreamManager{


/*
* interface to the exsting message types 
*/
class JethroMessage
{};


/*
* Request message type
*/
class JethroDataMessage::Request : public  JethroMessage
{};


/*
* Response message type
*/
class JethroDataMessage::Respond : public JethroMessage
{};


const UDWordType MAX_TRX_BUFFER_SZ= 2 * 0x100000; // 2Mb. should be from conf

/*
* Interface for Context storage
* this is used as an abstraction layer
* for concrete classes holding context [config?]
* data for the different implementations
*/
typedef struct tDataStreamContext
{}DataStreamContext;


/*
* Context for the DataInStreamMangr class
*/
typedef struct tDataInStreamContext : public DataStreamContext 
{}DataInStreamContext;


/*
* Context for the DataOutStreamMangr class
*/
typedef struct tDataOutStreamContext : DataStreamContext
{
  
 tDataOutStreamContext(const std::string& i_query):
                            m_query(i_query), 
			    m_rs(0){}
  /*query for which data is to be retreived */
  const std::string& m_query;
  
  /*resultSet from db */
  JethroResultSet const * m_rs;
}DataOutStreamContext;



/*
* this is an abstract class designed to hold _common_ code
* for the in and out strategies.
* any common functionality/member, should reside here.
* all virtual symbols that might have common code - first
* call their impl here, and then proceed to specific impls
*/
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

  /* perform stream finalization 
     in different strategies - this 
      will have different meaning*/
  virtual int finalizeStream(JethroMessage&) {/*perform some default cleanup*/}


  /* common members */
 private:

  TcpSocket* m_socket;

  DataStreamContext* m_args;
}


/*
* This is the "IN" interface. any stream manager wishing to
* add its own impl, needs to adhere to this interface.
*/
class DataInStreamMngr : public AbstractDataStreamMngr 
{
  virtual int handleProtoBuffRecv() = 0;
} 


/*
* This is the "OUT" interface. any stream manager wishing to
* add its own impl, needs to adhere to this interface.
*/
class DataOutStreamMngr : public AbstractDataStreamMngr
{
  virtual int handleProtoBuffSend() = 0;
} 

/*******************/
/* SPECIFIC IMPL'S */
/*******************/

/********************************/
/* THE BULK DATA STREAM MANAGER */
/********************************/


/*
* this class implements the "IN" interface with the
* option to break the stream read into bulks for 
* better usage of the protoBuff facility
*/
class BulkDataInStreanMngr : public DataInStreamMngr
{
 public:

  /*
  * C'tor
  * i_socket - tcp socket wrapper for comm
  * i_consumFunc - a functure designd for allowing
  *                buffer consumption IN THE CONTEXT
  *                of the client code [SocketClient in
  *                this specific example]
  */
  BulkDataInStreamMngr(TCPSocket * i_socket,
		       ConsumeFunc& i_consumFunc = s_pShowRows);
                                            

  /*Inits data that is built per request */
  virtual void init(DataStreamContext*);
  
  /*                                                          
  * main entry point for the receive logic                                      
  * TODO add algorithm
  */
  virtual int handleProtoBuffRecv();

 private:

  /* perform data receiving */
  int recvBulk();

  /*reseting state*/
  virtual void reset();

  /*
  * handles bulk finalization.
  * this is where metadata could be read  
  */
  virtual int finalizeStream(JethroMessage& i_protobufRespond, 
			     UDWordType i_count);

  /* a pointer to a member function and an obj instance
     this func is registered by the class using the streamMngr
     and should implement buffer consumption. it is done in
     the context of the client class. */
  ConsumeFunc& m_pConsum;
  
};



/*
* this class implements the "OUT" interface with the
* option to break the stream written into bulks for 
* better usage of the protoBuff facility
*/
class BulkDataOutStreamMngr : public DataOutStreamMngr
{

 public:


  /*
  * C'tor
  */
  BulkDataOutStreamMngr(TCPSocket * i_socket);

  /*Inits data that is built per request */
  virtual void init(DataStreamContext*);

  /*
  * main entry point for the send logic
  * TODO add algorithm
  */
  virtual int handleProtoBuffSend();

 private:

  /* performs data sending */
  int sendBulk(JethroMessage& i_protobufRespond,
	       SocketServer::PACKAGE_FLAGS_E i_flags);
  
  virtual void reset();

  virtual int finalizeStream(JethroMessage& i_protobufRespond, 
			     UDWordType i_count);
};


} // namespace JethroData
