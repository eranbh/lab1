#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

#include <string>
#include "GenTypes.h"

class SocketClient
{
public:
	SocketClient(std::string xi_serverAddress, short xi_port);

	~SocketClient(void);

	void sendQuery(const std::string xi_SqlQuery);

private:


	int consume(JethroDataMessage::Respond&);

	std::string m_serverAddress;

	short m_port;

	/* the stream manager for this class */
	DataInStreamMngr* m_datInStrmHndlr;

	// see GenTypes for def
	ConsumFunc* m_consFunc;
	
};

#endif
