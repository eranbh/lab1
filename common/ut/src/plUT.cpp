/*
 * plUT.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: eran
 */
#include "cppunit/extensions/HelperMacros.h"
#include "plUT.h" // my test cases
namespace pl{
	namespace ut{


CPPUNIT_TEST_SUITE_REGISTRATION(plUT);

void plUT::setUp(){}
void plUT::tearDown(){}

/*
* linux employs a system-wide strategy for pre-loading.
*
*/
void plUT::test_linux()
{

}

	} // namespace ut

} // namespace pl
