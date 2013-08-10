#include <iostream>
#include <iomanip>
#include "general.h"
#include "PracticalSocket.h"  
#include "xThread.h"
#include "JException.h"
#include "LocalBuffer.h"
#include "Jethro.h"
#include "JethroMessages.pb.h"
#include "SocketServer.h"

using namespace std;
using namespace JethroData;

typedef struct {
	SocketServer *socketServerObj;
	TCPSocket *sock;
} ThreadParams_St;

void executeThread(void *xi_argv);

JethroDataMessage::Column::ColumnType translateColumnType(JethroData::JethroResultSetMetaData::ColumnType_E xi_jethroColumnType);

SocketServer::SocketServer(void)
{
}


SocketServer::~SocketServer(void)
{
}

void SocketServer::Listen(int xi_port)
{
	try {
		int port = (xi_port ? xi_port : JETHRO_SOCKET_SERVER_DEFAULT_PORT);
		TCPServerSocket servSock(port);   // Socket descriptor for server  
  
		cout << "Listening on port: " << port << endl;

		for (;;) {      // Run forever  
			// Create separate memory for client argument  
			TCPSocket *clntSock = servSock.accept();
  
			// Create client thread  
			ThreadParams_St threadParams;
			threadParams.socketServerObj = this;
			threadParams.sock = clntSock;

			xThread::Begin(&executeThread, (void *) &threadParams, 0);
		}
	} catch (SocketException &e) {
		THROW_JEXCEPTION(0, "SocketServer::Listen - failure", e.what(), &e);
	} catch (exception &e) {
		THROW_JEXCEPTION(0, "SocketServer::Listen - failure", e.what(), &e);
	} catch (...) {
		THROW_JEXCEPTION(0,"SocketServer::Listen - failure", "unknow error", NULL);
	}
}

void SocketServer::executeTask(TCPSocket *xi_socket)
{
	if (m_queryDebugLevel >= BASE_DEBUG) cout << "start executeTask" << endl;
	// TODO: xThread::Detach();

	try {
		// unwarp tcp package:
		// -------------------

		// First - read package header

		if (m_queryDebugLevel >= BASE_DEBUG) cout << "read header" << endl;

		UDWordType recvBufferSize;
		PackageHeader_St header;

		UDWordType leftToRecv = sizeof(header);
		UDWordType totalRecvied = 0;
		while (leftToRecv!=0) {
			if ((recvBufferSize = xi_socket->recv((char *) (&header)+totalRecvied, leftToRecv))<=0) {
				cout << "Error - recv failed or connection disconnected" << endl;
				// TODO: error: return error - terminate thread 
				return;
			}
			totalRecvied += recvBufferSize;
			leftToRecv -= recvBufferSize;
			// TODO: what happens in package too small? it hangs. Take care of this case
		}
		// Convert from network endian to host endian
		header.PackageSize = TCPSocket::convNtohl(header.PackageSize);
		header.PackageFlags = TCPSocket::convNtohl(header.PackageFlags);

		if (m_queryDebugLevel >= BASE_DEBUG) cout << "Header read. Package size: " << header.PackageSize << endl;

		// Allocate memory for expected buffer size:
		//char *recvBuffer = new char[header.PackageSize];
		LocalBuffer<char> recvBufferObj(header.PackageSize);
		char *recvBuffer = recvBufferObj.getBuffer();

		// Next - read package data

		leftToRecv = header.PackageSize;
		totalRecvied = 0;
		while (leftToRecv!=0) {

			UDWordType maxPackageBufferSize = MAXIMUM_PACKAGE_BUFFER_SIZE; // move const to variable to avoid compailer error at min
			UDWordType bufRecvSize = min(leftToRecv, maxPackageBufferSize);

			if ((recvBufferSize = xi_socket->recv(recvBuffer+totalRecvied, bufRecvSize))<=0) {
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
		if (m_queryDebugLevel >= BASE_DEBUG) cout << "package recived. Size: " << totalRecvied << endl;

		// unwarp recived buffer using protobufs
		// ----------------
		
		
		JethroDataMessage::Request ProtobufRequest;
	
		if (!ProtobufRequest.ParseFromArray(recvBuffer, totalRecvied)) {
			// TODO: error: return error - terminate thread 
			cout << "Error - protobug parse request failure" << endl;
			return;
		}

		// Check protocol

		if (ProtobufRequest.type() != ProtobufRequest.QUERY) { //ERROR
			// TODO: error: return error - terminate thread 
			cout << "Error - invalid request type" << endl;
			return;
		}
		if (ProtobufRequest.query().format() != ProtobufRequest.query().SQL) { //ERROR
			// TODO: error: return error - terminate thread 
			cout << "Error - invalid query type" << endl;
			return;
		}
	
		// set the default output format:
		/* -- 
		// TODO: temporary removed to allow control of the format from global params. 
		//       To be changes when format control is done from client

		g_trimOutput = false;
		g_outputAsCSV = true;			
		// change output format is request has output format:
		if (ProtobufRequest.query().has_outputformat()) {
			switch (ProtobufRequest.query().outputformat()) {
			case JethroDataMessage::Query::CSV :
				g_trimOutput = false;
				g_outputAsCSV = true;				
				break;
			case JethroDataMessage::Query::FORMATED :
				g_trimOutput = true;
				g_outputAsCSV = false;				
				break;
			}
		} 
		*/
		// some settings for output:


		// Get the query

		const string SqlQuery = ProtobufRequest.query().sqlquery();

		cout << xxGeneral::getLocalTime() << endl;
		cout << "Recived: " << SqlQuery << endl;

		// execute query
		// -------------
	
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
		  
	} catch (exception &e) {

		cout << "Server task failed due to: " << e.what() << endl;

	} catch (...) {

		cout << "Server task failed" << endl;
		// TODO: handle error but don't go before delete socket - we want to close the connection
	}

	delete xi_socket;

}

void executeThread(void *xi_argv)
{
	ThreadParams_St *xi_params = (ThreadParams_St *) xi_argv;


	xi_params->socketServerObj->executeTask(xi_params->sock);


}

// Helper func - TODO: move to future protocol tools 
JethroDataMessage::Column::ColumnType translateColumnType(JethroData::JethroResultSetMetaData::ColumnType_E xi_jethroColumnType)
{
	JethroDataMessage::Column::ColumnType type = JethroDataMessage::Column::NONE;
	switch (xi_jethroColumnType) {
	case JethroData::JethroResultSetMetaData::BIGINT :
		type = JethroDataMessage::Column::BIGINT;
		break;
	case JethroData::JethroResultSetMetaData::BINARY :
		type = JethroDataMessage::Column::BINARY;
		break;
	case JethroData::JethroResultSetMetaData::BLOB :
		type = JethroDataMessage::Column::BLOB;
		break;
	case JethroData::JethroResultSetMetaData::CLOB :
		type = JethroDataMessage::Column::CLOB;
		break;
	case JethroData::JethroResultSetMetaData::CHAR :
		type = JethroDataMessage::Column::CHAR;
		break;
	case JethroData::JethroResultSetMetaData::DATE :
		type = JethroDataMessage::Column::DATE;
		break;
	case JethroData::JethroResultSetMetaData::DECIMAL :
		type = JethroDataMessage::Column::DECIMAL;
		break;
	case JethroData::JethroResultSetMetaData::DOUBLE :
		type = JethroDataMessage::Column::DOUBLE;
		break;
	case JethroData::JethroResultSetMetaData::INTEGER :
		type = JethroDataMessage::Column::INTEGER;
		break;
	case JethroData::JethroResultSetMetaData::REAL :
		type = JethroDataMessage::Column::REAL;
		break;
	case JethroData::JethroResultSetMetaData::SMALLINT :
		type = JethroDataMessage::Column::SMALLINT;
		break;
	case JethroData::JethroResultSetMetaData::TIME :
		type = JethroDataMessage::Column::TIME;
		break;
	case JethroData::JethroResultSetMetaData::TIMESTAMP :
		type = JethroDataMessage::Column::TIMESTAMP;
		break;
	case JethroData::JethroResultSetMetaData::VARCHAR :
		type = JethroDataMessage::Column::VARCHAR;
		break;

	}
	return type;
}
