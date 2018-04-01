
#include "inc/ClientUiTask.h"
/*
* this is the main driver of the entire system.
* I placed the client logic in its own task. this was done
* in order to support an easy addition of a _server_ task.
* why would that be a good idea?
* 1. UI thread must never block. if, for example, the user database
*    gets very large, servicing each request might slow the UI thread down.
* 2. persistence of data: if that is ever a requirement, the thread servicing
*    the requests _must_ be detached from the one facing the user.
*/

int main(int, const char* [])
{
    msg_store::ClientUiTask client;
    client.startThread();
    client.waitForUiThread();

    // server.startThread(); // not impl'ed
    // server.waitForUiThread(); // not impl'ed

	
	return 0;
}
