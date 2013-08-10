// JethroServer.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <string>
#include "general.h"
#include "SocketServer.h"
#include "Jethro.h"

using namespace std;
using namespace JethroData;

int main(int argc, char* argv[])
{
	if (argc < 3) {
		cout << "Usage: JethroServer [scheme file path] [param file path] {port(=9111)}" << endl;
		exit(0);
	}

	string schemeFilePath = argv[1];
	string paramsFilePath = argv[2];
	int port = 0;
	if (argc >= 4) port = atoi(argv[3]);

	try {

		Jethro *JethroObj = Jethro::getInstance(schemeFilePath, paramsFilePath);

		cout << "Ready - " << xxGeneral::getLocalTime() << endl;

		SocketServer socketServer;

		socketServer.Listen(port);

	} catch (exception &e) {
		cout << "JetroServer failure: " << e.what() << endl;
		exit(1);
	}

}

