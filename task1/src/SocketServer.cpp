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


		/***********************************************************/
		/*             stream handler start                        *
		/***********************************************************/

		DataOutStreamContext outCtx(SqlQuery);
		m_datOutStrmHndlr.init(&outCtx);
		m_datOutStrmHndlr.handleProtoBuffSend();
		m_datOutStrmHndlr.reset();

		/***********************************************************/
		/*             stream handler end                          *
		/***********************************************************/
	
		  
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
