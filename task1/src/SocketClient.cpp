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
  m_consFunc = new ConsumFunc(boost::bind(&SocketClient::consume, this, _1);
  m_datInStrmHndlr = new BulkDataInStreamMngr(sock, *m_consumFunc);
}


SocketClient::~SocketClient(void)
{
  delete m_datInStrmHndl;
  m_datInStrmHndl = 0;
  delete m_consFunc;
  m_consFunc = 0;
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
		

	  /***********************************************************/
	  /*             stream handler start                        *
	  /***********************************************************/
         
          /* assumption: this class will not be shared among threads
             if that is false, the below code might cause undefined 
             behavior, as the ctx is added to the manager by every thread */

	  DataInStreamContext inCtx(/* init buffer level params */);
	  m_datInStrmHndlr->init(&InCtx); /* not thread safe */
	  m_datInStrmHndlr->handleProtoBuffSend();
	  m_datInStrmHndlr->reset();
	  /***********************************************************/
	  /*             stream handler end                          *
	  /***********************************************************/

  } catch (SocketException &e) {
    cerr << "Socket failure " << e.what() << endl;
    return;
  } catch (exception &e) {
    cerr << "Client send failure " << e.what() << endl;
    return;
  }
  return;
}



/*
* this func's purpose is to consume the buffer.
* as the dataStreamManager is allowed to consume the
* data during the stream handling, this func is bound
* to a functure that is also "visible" to the manager.
*/
int
SocketClient::consume(JethroDataMessage::Respond& protobufRespond)
{
  switch(protobufRespond.type()) {
  case (JethroDataMessage::Respond::STATUS) :
    if (protobufRespond.status() == JethroDataMessage::Respond::FAILURE) {
      cerr << protobufRespond.error().errormessage() << endl;
    }
    return;
    break;
  case (JethroDataMessage::Respond::RESULT) :

    // we can access metadata here, as we are sending an intermediate
    // sum on every bulk AS WELL as the grand total
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
	
}
