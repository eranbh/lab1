/*ADD STUFF*/
#include "DataStreamManager.h"


namespace JethroData{
  namespace DataStreamManager{




/**********************/
/*BulkDataInStreamMngr*/
/**********************/

/*
* C'tor
*/
BulkDataInStreamMngr::
BulkDataInStreamMngr(TCPSocket * i_socket,
		     ConsumeFunc& i_consFunc,
                     AbstractDataStreamMngr(i_socket),
                     m_pConsum(i_consFunc)
{
}


/*                                                          
* main entry point for the receive logic                                      
* TODO add algorithm
*/
int
BulkDataInStreanMngr::handleProtoBuffRecv()
{
  
  do /* attempt a read at least once */
  {

    UDWordType recvBufferSize;
    SocketServer::PackageHeader_St header;

    UDWordType leftToRecv = sizeof(header);
    UDWordType totalRecvied = 0;
    while (leftToRecv!=0) {
      if ((recvBufferSize = m_sock.recv((char *) (&header)+totalRecvied, leftToRecv))<=0) {
	cout << "Error - recv failed or connection disconnected" << endl;
	// TODO: error: return error - terminate thread 
	return;
      }
      totalRecvied += recvBufferSize;
      leftToRecv -= recvBufferSize;
      // TODO: what happens in package too small? it hangs. Take care of this case
    }
    //cout << "Header read. Package size: " << header.PackageSize << endl;
    // Convert from network endian to host endian
    header.PackageSize = TCPSocket::convNtohl(header.PackageSize);
    header.PackageFlags = TCPSocket::convNtohl(header.PackageFlags);

    // Next -read result

    //cout << "Header read. Package size: " << header.PackageSize << endl;

    // Allocate memory for expected buffer size:
    LocalBuffer<char> recvBufferObj(header.PackageSize);
    char *recvBuffer = recvBufferObj.getBuffer();
    //char *recvBuffer = new char[header.PackageSize];

    // Next - read package data

    leftToRecv = header.PackageSize;
    totalRecvied = 0;
    while (leftToRecv!=0) {

      UDWordType maxPackageBufferSize = SocketServer::MAXIMUM_PACKAGE_BUFFER_SIZE; // move const to variable to avoid compailer error at min
      UDWordType bufRecvSize = min(leftToRecv, maxPackageBufferSize);
      //cout << "leftToRecv: " << leftToRecv << " totalRecvied: " << totalRecvied << endl;
      if ((recvBufferSize = m_sock.recv(recvBuffer+totalRecvied, bufRecvSize))<=0) {
	cout << "Error - recv failed or connection disconnected" << endl;
	// TODO: error: return error - terminate thread 
	return;
      }
      totalRecvied += recvBufferSize;
      leftToRecv -= recvBufferSize;
      // TODO: what happens in package too small? it hangs. Take care of this case
    }
    
    JethroDataMessage::Respond protobufRespond;
	
    if (!protobufRespond.ParseFromArray(recvBuffer, totalRecvied)) {
      // TODO: error: return error - terminate thread 
      cout << "Error - protobug parse respond failure" << endl;
      return;
    }
    
    /* consume the intermediate buffer */
    m_pConsum(protobufRespond);

  }while(header.PackageFlags & SocketServer::MSG_CONT);  // we stop when the message we read had no CONT

  finalizeStream(protobufRespond, i_count); // last buffer contains per resultSet data
 
}

int
BulkDataInStreanMngr::finalizeStream(JethroMessage& i_protobufRespond, UDWordType sz)
{

  AbstractDataStreamMngr::finalizeStream(i_protobufRespond,sz);

  /* do something with the last message ?? */
  
  return 0;
}



 /*Inits data that is built per request */
void BulkDataInStreanMngr::init(DataStreamContext* i_ctx)
{ 
   AbstractDataStreamMngr::init(i_ctx);
   /* perform specific init */ 
}


/*reseting state*/
void 
BulkDataInStreanMngr::reset()
{
  AbstractDataStreamMngr::reset();
  /* perform specific reset */
}



/***********************/
/*BulkDataOutStreamMngr*/
/***********************/


/*
* C'tor
* Data that is built once per socket
*/
BulkDataOutStreamMngr::
BulkDataOutStreamMngr(TCPSocket * i_socket):
                                  AbstractDataStreamMngr(i_socket)
{}



/*Inits data that is built per request */
void
BulkDataOutStreamMngr::init(DataStreamContext* i_ctx)
{
  AbstractDataStreamMngr::init(i_ctx);
  /* perform  specific init  */
  
}

/*reseting state*/
void 
BulkDataOutStreamMngr::reset()
{
  AbstractDataStreamMngr::reset();
  /* perform  specific reset */
}



/*
* main entry point for the send logic
* TODO add algorithm
*/
int
BulkDataOutStreamMngr::handleProtoBuffSend()
{

  // execute query
  // -------------


  JethroDataMessage::Respond protobufRespond;
  JethroResultSet const *resultSet = NULL;

  // take query from ctx
  const std::string& sqlQuery = 
              (static_cast<DataOutStreamArgs*>(m_args))->m_query;
  
  try {
    
    resultSet = Jethro::getInstance()->executeQuery(sqlQuery);
    if (resultSet == NULL) { // TODO : temp - change to handle instruction by recive instruciton request
      protobufRespond.set_type(JethroDataMessage::Respond::STATUS);
      protobufRespond.set_serialno(1); //TODO: expend this

    } else {

      // add the resultSet to our context - we will need it later
      static_cast<DataOutStreamContext*>(m_args)->m_rs = resultSet;
      
      
      cout << "Rows count: " << resultSet->getResultsSetSize() << endl;
      
      // prepare result protobuf and send results back

      protobufRespond.set_type(JethroDataMessage::Respond::RESULT);//  RespontType::Respond_RespontType_RESULT);
      protobufRespond.set_serialno(1); //TODO: expend this
      protobufRespond.mutable_metadata()->set_rowscount(resultSet->getResultsSetSize());

      // TODO:	JethroDataMessage::Column *column = protobufRespond.mutable_metadata()->add_columns();

      protobufRespond.mutable_rows()->set_type(JethroDataMessage::RowSet::CSV);// RowSetType::RowSet_RowSetType_CSV);
      UDWordType count=0;
      UDWordType totalBulkSz=0;
      SocketServer::PackageFlags_E flags = |= SocketServer::MSG_CONT | SocketServer::PROTOCOL_PROTOBUF;

       // "breaking-loop" pattern as we want to call send from one logical path
      // we never leave this loop unless all data is transmitted
      while (!resultSet->isEndOfSet()) {
	while(totalBulkSz  <  MAX_TRX_BUFFER_SZ &&
	      !resultSet->isEndOfSet()) // extra careful not to cross the limit
	{
	  boost::shared_ptr<string> CSVRow (resultSet->getNextCSVRow()); // auto discard
	  protobufRespond.mutable_rows()->add_csvrow(CSVRow->c_str(), CSVRow->length());
	  totalBulkSz += CSVRow->length();
	  count++;
	} // while 2

	// time to send bulk. first make sure we dont need to finalize
	// this is ugly, but prevents the need for a special tcp message
        // with only the metadata. 
	if(resultSet->isEndOfSet()) /* TODO put branch prediction hint [not likely] */
	{
	  finalizeStream(protobufRespond, count);
	  flags = SocketServer::PROTOCOL_PROTOBUF; /* indicating last message */
	}
	
	sendBulk(protobufRespond, flags); /* this is the _only_ sending point */
	totalBulkSz=0;
	protobufRespond.clear(); // im assuming such a func exists
      } // while 1
	 
    } // else
  
  } catch (exception &e) {

    cout << "failed with: " << e.what() << endl;
    protobufRespond.set_type(JethroDataMessage::Respond::STATUS);// RespontType::Respond_RespontType_STATUS);
    protobufRespond.set_status(JethroDataMessage::Respond::FAILURE);// Status::Respond_Status_FAILURE);
    protobufRespond.mutable_error()->set_errorcode(1);
    protobufRespond.mutable_error()->set_errormessage(e.what(), strlen(e.what()));

  } catch (...) {

    cout << "failed with unknow error" << endl;
    protobufRespond.set_type(JethroDataMessage::Respond::STATUS);// RespontType::Respond_RespontType_STATUS);
    protobufRespond.set_status(JethroDataMessage::Respond::FAILURE);// Status::Respond_Status_FAILURE);
    protobufRespond.mutable_error()->set_errorcode(1);
    string errorMessage = "Unknow error when executing query";
    protobufRespond.mutable_error()->set_errormessage(errorMessage.c_str(), errorMessage.length());

  }

  if (resultSet) delete resultSet;	
}


int
BulkDataOutStreamMngr::finalizeStream(JethroMessage& i_protobufRespond, UDWordType sz)
{
  AbstractDataStreamMngr::finalizeStream(i_protobufRespond, sz);
    
  JethroDataMessage::Respond&  protobufRespond = 
		  static_cast<JethroDataMessage::Respond&>(i_protobufRespond);
    
    protobufRespond.mutable_rows()->set_size(sz);

    protobufRespond.mutable_metadata()->mutable_querytime()->set_query(ctx->m_rs->getQueryTimeAbs());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_querycpu(ctx->m_rs->getQueryCpuTime());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_getdata(ctx->m_rs->getGetDataTimeAbs());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_getdatacpu(ctx->m_rs->getGetDataCpuTime());

    protobufRespond.mutable_metadata()->mutable_querytime()->set_total(ctx->m_rs->getQueryTimeAbs() + ctx->m_rs->getGetDataTimeAbs());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_totalcpu(ctx->m_rs->getQueryCpuTime() + ctx->m_rs->getGetDataCpuTime());

    if (m_queryDebugLevel >= BASE_DEBUG) cout << "resultSet->getMetaData().size(): " << ctx->m_rs->getMetaData().size() << endl;
    for(int i=0; i < ctx->m_rs->getMetaData().size(); i++) {
      JethroDataMessage::Column *columnMetaData = protobufRespond.mutable_metadata()->add_columns();
      columnMetaData->set_label(ctx->m_rs->getMetaData().getColumnLabel(i));					
      columnMetaData->set_name(ctx->m_rs->getMetaData().getColumnName(i));					
      columnMetaData->set_type(translateColumnType(ctx->m_rs->getMetaData().getColumnType(i)));
      columnMetaData->set_tablename(ctx->m_rs->getMetaData().getTableName(i));					
    }
    
    // Print query time to output 
    cout << endl << "Total query time: " << fixed << setprecision(3) << "[" << protobufRespond.metadata().querytime().total() << "]";
    cout << " (" << setprecision(3) << protobufRespond.metadata().querytime().totalcpu() << ")";
    cout << " - execute: " << setprecision(3) << protobufRespond.metadata().querytime().query();
    cout << " (" << setprecision(3) << protobufRespond.metadata().querytime().querycpu() << ")";
    cout << " show data: " << setprecision(3) << protobufRespond.metadata().querytime().getdata();
    cout << " (" << setprecision(3) << protobufRespond.metadata().querytime().getdatacpu() << ")";
    cout << endl << endl;
}


int BulkDataOutStreamMngr::sendBulk(JethroDataMessage::Respond& protobufRespond,
				    SocketServer::PACKAGE_FLAGS_E i_flags)
{
    // send

  UDWordType totalSentBufferSize = sizeof(SocketServer::PackageHeader_St) + protobufRespond.ByteSize();

  LocalBuffer<char> sentBufferObj(totalSentBufferSize);
  char *sentBuffer = sentBufferObj.getBuffer();
  //char *sentBuffer = new char[totalSentBufferSize];
  char *sentBufferPtr = sentBuffer + sizeof(SocketServer::PackageHeader_St);

  protobufRespond.SerializeToArray(sentBufferPtr,  protobufRespond.ByteSize());

  // pack the protobuf message in jethro package:
  SocketServer::PackageHeader_St *headerSend = (SocketServer::PackageHeader_St *) sentBuffer;

  headerSend->PackageFlags = i_flags; // added the new options here
  headerSend->PackageSize = protobufRespond.ByteSize();
  if (m_queryDebugLevel >= BASE_DEBUG) cout << "sending total of " << totalSentBufferSize << " bytes. Package size: " << headerSend->PackageSize << endl;

  // Convert from host endian to network endian
  headerSend->PackageSize = TCPSocket::convHtonl(headerSend->PackageSize);
  headerSend->PackageFlags = TCPSocket::convHtonl(headerSend->PackageFlags);


  // Send the string to the echo server
  UDWordType sentBufferSizeLeft = totalSentBufferSize;
  UDWordType totalSent = 0;
  UDWordType maxPackageSize = SocketServer::MAXIMUM_PACKAGE_BUFFER_SIZE;
  while (sentBufferSizeLeft > 0) {
    UDWordType sizeToSend = min(sentBufferSizeLeft, maxPackageSize);
    xi_socket->send(sentBuffer+totalSent, sizeToSend);
    sentBufferSizeLeft -= sizeToSend;
    totalSent += sizeToSend;
  }

  if (m_queryDebugLevel >= BASE_DEBUG) cout << "Send completed" << endl; // DEBUG

}



   } // namespace DataStreamManager
} // namespace JethroData
