#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include "GenTypes.h"
#include "PracticalSocket.h"

class SocketServer
{
public:
	SocketServer(void);

	~SocketServer(void);

	void Listen(int xi_port = JETHRO_SOCKET_SERVER_DEFAULT_PORT);

	void executeTask(TCPSocket *xi_socket);

private:

	static const int JETHRO_SOCKET_SERVER_DEFAULT_PORT = 9111;

	typedef struct {
		DWordType PackageSize;
		DWordType PackageFlags;
	} PackageHeader_St;

	enum PackageFlags_E {PROTOCOL_PROTOBUF = 0x00000001};

	static const DWordType MAXIMUM_PACKAGE_BUFFER_SIZE = 1024;

	friend class SocketClient;
};


#endif
