/*
* This should should be used as the main
* for any test suit wishing to use cppunit
*/
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/CompilerOutputter.h"
#include "cppunit/ui/text/TestRunner.h"
#include "cppunit/XmlOutputter.h"

int main(int argc, char* argv[])
{
  CppUnit::TextUi::TestRunner runner;

  // always an output
  runner.setOutputter(new CppUnit::XmlOutputter(&runner.result(), 
                                                     std::cout));
  CppUnit::TestFactoryRegistry &registry =
             CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  
  bool wasSuccessful = runner.run("" , false);  
  return wasSuccessful ? 0 : 1;   
}
