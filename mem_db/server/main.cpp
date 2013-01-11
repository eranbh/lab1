/*
* Servers main file
*/
#include "flow_manager.h"
int main(int argc, char* argv[])
{
  FlowMngr mngr;
  mngr.activate();
  mngr.waitForAll();
  return 0;
}
