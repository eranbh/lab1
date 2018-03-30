
#include "inc/ClientUiTask.h"

int main(int, const char* [])
{
    msg_store::ClientUiTask client;
    client.startThread();
	
	return 0;
}
