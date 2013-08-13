/*ADD STUFF*/
#include "DataStreamManager.h"


namespace JethroData{
  namespace DataStreamManager{
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
  
  do /* at least one bulk to read */
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
    /*
      if (totalRecvBufferSize < header.PackageSize) {
      // TODO: error: return error - terminate thread 
      cout << "Error - package size (" << totalRecvBufferSize << ") mismatch expected buffer size (" << header.PackageSize << ")." << endl;
      return;
      }
    */
    //cout << "package recived. Size: " << totalRecvied << endl;

    // Unwarp response protobuf package
    // ================================

    JethroDataMessage::Respond protobufRespond;
	
    if (!protobufRespond.ParseFromArray(recvBuffer, totalRecvied)) {
      // TODO: error: return error - terminate thread 
      cout << "Error - protobug parse respond failure" << endl;
      return;
    }
    
    /* consume the buffer in the SocketClient's ctx */
    m_pConsum(protobufRespond);

  }while(header.PackageFlags & SocketServer::MSG_CONT); // maybe this a dependancy we dont want
                                                         // consider moving the enum to a mutual header

  finalizeStream(&protobufRespond, i_count); // last buffer contains per resultSet data
 
}

int
BulkDataInStreanMngr::finalizeStream(JethroMessage& i_protobufRespond, UDWordType i_count)
{
  
  /* extract metadata and consume it */
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

  
  assert(m_args && "per bulk args not set");
  assert(false == static_cast<DataOutStreamArgs*>(m_args))->m_query.empty() &&
         "query not supplied");
  const std::string& sqlQuery = 
              (static_cast<DataOutStreamArgs*>(m_args))->m_query;
  
  try {
    
    resultSet = Jethro::getInstance()->executeQuery(sqlQuery);
    if (resultSet == NULL) { // TODO : temp - change to handle instruction by recive instruciton request
      protobufRespond.set_type(JethroDataMessage::Respond::STATUS);
      protobufRespond.set_serialno(1); //TODO: expend this

    } else {

      // add the resultSet to our context
      static_cast<DataOutStreamContext*>(m_args)->m_rs = resultSet;
      resultSet
      
      cout << "Rows count: " << resultSet->getResultsSetSize() << endl;
      
      // prepare result protobuf and send results back

      protobufRespond.set_type(JethroDataMessage::Respond::RESULT);//  RespontType::Respond_RespontType_RESULT);
      protobufRespond.set_serialno(1); //TODO: expend this
      protobufRespond.mutable_metadata()->set_rowscount(resultSet->getResultsSetSize());

      // TODO:	JethroDataMessage::Column *column = protobufRespond.mutable_metadata()->add_columns();

      protobufRespond.mutable_rows()->set_type(JethroDataMessage::RowSet::CSV);// RowSetType::RowSet_RowSetType_CSV);
      UDWordType count=0;
      UDWordType totalBulkSz=0;

      while (!resultSet->isEndOfSet()) { //TODO: stop when reached limit, send the buf and create a new buf for next rows
	string *CSVRow = resultSet->getNextCSVRow();
	protobufRespond.mutable_rows()->add_csvrow(CSVRow->c_str(), CSVRow->length());
	totalBulkSz += CSVRow->length();
	delete CSVRow;
	count++;
	if(totalBulkSz >=  MAX_TRX_BUFFER_SZ) // 
	{
	  // time to send bulk
	  sendBulk(protobufRespond, SocketServer::PROTOCOL_PROTOBUF|MSG_CONT);
	  totalBulkSz=0;
	}
	// add the metadata to payload of last message
	finalizeStream(&protobufRespond, count);
	sendBulk(protobufRespond, SocketServer::PROTOCOL_PROTOBUF);
	// this is done here as well as in the finalize, as this is taken by
        // the client code on EVERY bulk, and not just at the end [total]
	protobufRespond.mutable_rows()->set_size(++count);
      }
    }
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
BulkDataOutStreamMngr::finalizeStream(JethroMessage& i_protobufRespond, UDWordType i_count)
{
    AbstractDataStreamMngr::finalizeStream();
    protobufRespond.mutable_rows()->set_size(i_count);

    protobufRespond.mutable_metadata()->mutable_querytime()->set_query(resultSet->getQueryTimeAbs());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_querycpu(resultSet->getQueryCpuTime());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_getdata(resultSet->getGetDataTimeAbs());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_getdatacpu(resultSet->getGetDataCpuTime());

    protobufRespond.mutable_metadata()->mutable_querytime()->set_total(resultSet->getQueryTimeAbs() + resultSet->getGetDataTimeAbs());
    protobufRespond.mutable_metadata()->mutable_querytime()->set_totalcpu(resultSet->getQueryCpuTime() + resultSet->getGetDataCpuTime());

    if (m_queryDebugLevel >= BASE_DEBUG) cout << "resultSet->getMetaData().size(): " << resultSet->getMetaData().size() << endl;
    for(int i=0; i < resultSet->getMetaData().size(); i++) {
      JethroDataMessage::Column *columnMetaData = protobufRespond.mutable_metadata()->add_columns();
      columnMetaData->set_label(resultSet->getMetaData().getColumnLabel(i));					
      columnMetaData->set_name(resultSet->getMetaData().getColumnName(i));					
      columnMetaData->set_type(translateColumnType(resultSet->getMetaData().getColumnType(i)));
      columnMetaData->set_tablename(resultSet->getMetaData().getTableName(i));					
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


int BulkDataOutStreamMngr::sendBulk(JethroMessage& i_protobufRespond,
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
