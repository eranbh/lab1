#include <string>
#include <iostream>
#include <iomanip>
#include "general.h"
#include "JethroMessages.pb.h"
#include "LocalBuffer.h"
#include "SocketServer.h"
#include "SocketClient.h"

using namespace std;

SocketClient::SocketClient(string xi_serverAddress, short xi_port) : m_serverAddress(xi_serverAddress), m_port(xi_port)
{
}


SocketClient::~SocketClient(void)
{
}

void SocketClient::sendQuery(const std::string xi_SqlQuery)
{
	try {
		// Establish connection with the echo server
		TCPSocket sock(m_serverAddress, m_port);

		// Pack the query in protobuf:
		JethroDataMessage::Request protobufRequest;

		protobufRequest.set_type(JethroDataMessage::Request::QUERY);
		protobufRequest.mutable_query()->set_format(JethroDataMessage::Query::SQL);
		protobufRequest.mutable_query()->set_sqlquery(xi_SqlQuery.c_str(), xi_SqlQuery.length());
		protobufRequest.mutable_query()->set_outputformat(JethroDataMessage::Query::FORMATED); // Show results as formated rows

		UDWordType totalSentBufferSize = sizeof(SocketServer::PackageHeader_St) + protobufRequest.ByteSize();

		LocalBuffer<char> sentBufferObj(totalSentBufferSize);
		char *sentBuffer = sentBufferObj.getBuffer();
		//char *sentBuffer = new char[totalSentBufferSize];
		char *sentBufferPtr = sentBuffer + sizeof(SocketServer::PackageHeader_St);

		protobufRequest.SerializeToArray(sentBufferPtr,  protobufRequest.ByteSize());

		// pack the protobuf message in jethro package:
		SocketServer::PackageHeader_St *headerSend = (SocketServer::PackageHeader_St *) sentBuffer;

		headerSend->PackageFlags = SocketServer::PROTOCOL_PROTOBUF;
		headerSend->PackageSize = protobufRequest.ByteSize();
  		//cout << "sending total of " << totalSentBufferSize << " bytes. Package size: " << headerSend->PackageSize << endl;
		// Convert from host endian to network endian
		headerSend->PackageSize = TCPSocket::convHtonl(headerSend->PackageSize);
		headerSend->PackageFlags = TCPSocket::convHtonl(headerSend->PackageFlags);

		// Send the string to the echo server
		UDWordType sentBufferSizeLeft = totalSentBufferSize;
		UDWordType totalSent = 0;
		UDWordType maxPackageSize = SocketServer::MAXIMUM_PACKAGE_BUFFER_SIZE;
		while (sentBufferSizeLeft > 0) {
			UDWordType sizeToSend = min(sentBufferSizeLeft, maxPackageSize);
			sock.send(sentBuffer+totalSent, sizeToSend);
			sentBufferSizeLeft -= sizeToSend;
			totalSent += sizeToSend;
		}

		//cout << "Send completed" << endl; // DEBUG

		// recv answer
		// ===========

		// First - read package header

		UDWordType recvBufferSize;
		SocketServer::PackageHeader_St header;

		UDWordType leftToRecv = sizeof(header);
		UDWordType totalRecvied = 0;
		while (leftToRecv!=0) {
			if ((recvBufferSize = sock.recv((char *) (&header)+totalRecvied, leftToRecv))<=0) {
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
			if ((recvBufferSize = sock.recv(recvBuffer+totalRecvied, bufRecvSize))<=0) {
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

		switch(protobufRespond.type()) {
		case (JethroDataMessage::Respond::STATUS) :
			if (protobufRespond.status() == JethroDataMessage::Respond::FAILURE) {
				cerr << protobufRespond.error().errormessage() << endl;
			}
			return;
			break;
		case (JethroDataMessage::Respond::RESULT) :

			UQWordType rowsCount = protobufRespond.metadata().rowscount();

			// show rows
			UQWordType count = 0;
			cout << endl;
			while (count < protobufRespond.rows().size()) {
				cout << protobufRespond.rows().csvrow(count++) << endl;
			}

			//TODO: support output format parameters. If output is CSV don't print this:
			cout << endl << "Rows count: " << rowsCount << endl;

			if (protobufRespond.metadata().has_querytime()) {

				cout << endl << "Total query time: " << fixed << setprecision(3) << "[" << protobufRespond.metadata().querytime().total() << "]";
				cout << " (" << setprecision(3) << protobufRespond.metadata().querytime().totalcpu() << ")";
				cout << " - execute: " << setprecision(3) << protobufRespond.metadata().querytime().query();
				cout << " (" << setprecision(3) << protobufRespond.metadata().querytime().querycpu() << ")";
				cout << " show data: " << setprecision(3) << protobufRespond.metadata().querytime().getdata();
				cout << " (" << setprecision(3) << protobufRespond.metadata().querytime().getdatacpu() << ")";
				cout << endl << endl;					

			}

			break;
		}


	} catch (SocketException &e) {
		cerr << "Socket failure " << e.what() << endl;
		return;
	} catch (exception &e) {
		cerr << "Client send failure " << e.what() << endl;
		return;
	}
	return;
}
