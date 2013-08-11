/*ADD STUFF*/
#include "DataStreamManager.h"


namespace JethroData{


/*
* C'tor
* Data that is built once per socket
*/
BulkDataInStreanMngr::
BulkDataInStreamMngr(TCPSocket * i_socket):AbstractDataStreamMngr(i_socket)
{
  assert(m_socket);
}


/*                                                          
* main entry point for the receive logic                                      
* TODO add algorithm
*/
int
BulkDataInStreanMngr::handleProtoBuffRecv()
{}

int
BulkDataInStreanMngr::finalizeStream(){}


/*Inits data that is built per request */
void 
BulkDataInStreanMngr::init(char* i_recvBuff)
{}


/*reseting state*/
void 
BulkDataInStreanMngr::reset(){}



/*
* C'tor
* Data that is built once per socket
*/
BulkDataOutStreamMngr::
BulkDataOutStreamMngr(TCPSocket * i_socket):AbstractDataStreamMngr(i_socket)
{
  assert(m_socket);
}


/*
* main entry point for the send logic
* TODO add algorithm
*/
int
BulkDataOutStreamMngr::handleProtoBuffSend()
{

  JethroDataMessage::Respond protobufRespond;
  JethroResultSet const *resultSet = NULL;
  
  try {
    
    resultSet = Jethro::getInstance()->executeQuery(SqlQuery);
    if (resultSet == NULL) { // TODO : temp - change to handle instruction by recive instruciton request
      protobufRespond.set_type(JethroDataMessage::Respond::STATUS);
      protobufRespond.set_serialno(1); //TODO: expend this

    } else {
      
      cout << "Rows count: " << resultSet->getResultsSetSize() << endl;
      
      // prepare result protobuf and send results back

      protobufRespond.set_type(JethroDataMessage::Respond::RESULT);//  RespontType::Respond_RespontType_RESULT);
      protobufRespond.set_serialno(1); //TODO: expend this
      protobufRespond.mutable_metadata()->set_rowscount(resultSet->getResultsSetSize());

      // TODO:	JethroDataMessage::Column *column = protobufRespond.mutable_metadata()->add_columns();

      protobufRespond.mutable_rows()->set_type(JethroDataMessage::RowSet::CSV);// RowSetType::RowSet_RowSetType_CSV);
      UDWordType count=0;
      while (!resultSet->isEndOfSet()) { //TODO: stop when reached limit, send the buf and create a new buf for next rows
	string *CSVRow = resultSet->getNextCSVRow();
	protobufRespond.mutable_rows()->add_csvrow(CSVRow->c_str(), CSVRow->length());
	delete CSVRow;
	count++;
	//protobufRespond.mutable_rows()->set_size(++count);
      }
      protobufRespond.mutable_rows()->set_size(count);

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

  // send

  UDWordType totalSentBufferSize = sizeof(SocketServer::PackageHeader_St) + protobufRespond.ByteSize();

  LocalBuffer<char> sentBufferObj(totalSentBufferSize);
  char *sentBuffer = sentBufferObj.getBuffer();
  //char *sentBuffer = new char[totalSentBufferSize];
  char *sentBufferPtr = sentBuffer + sizeof(SocketServer::PackageHeader_St);

  protobufRespond.SerializeToArray(sentBufferPtr,  protobufRespond.ByteSize());

  // pack the protobuf message in jethro package:
  SocketServer::PackageHeader_St *headerSend = (SocketServer::PackageHeader_St *) sentBuffer;

  headerSend->PackageFlags = SocketServer::PROTOCOL_PROTOBUF;
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


int
BulkDataOutStreamMngr::finalizeStream()
{}


/*Inits data that is built per request */
void
BulkDataOutStreamMngr::init(char* i_recvBuff)
{}


void 
BulkDataOutStreamMngr::reset()
{}
  

} // namespace JethroData
