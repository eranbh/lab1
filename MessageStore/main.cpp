
#include "inc/ClientUiTask.h"

int main(int, const char* [])
{
    msg_store::ClientUiTask client;
    client.startThread();
    client.waitForUiThread();
	
	return 0;
}