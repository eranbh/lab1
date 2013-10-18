/*
 * plUT.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: eran
 *
 *  This test suit has a very important prerequisite:
 *  	It has to have LD_PRELOAD [or its Unix equivalents]
 *  	pointing to liboit_stubs.so
 *  	If this is not the case, many [if not all] test cases
 *  	will fail miserably.
 *  	1. Launch from eclipse:
 *  		add the path to the env tab of the
 *  		execution/project/IDE.
 *  		Note: you have an ooo execution called plUT for that.
 *      2. Launch from shell
 *         	export the var into the session/add it to you .profile
 *         	Note: you have a script for that: plUT.sh
 *     	This also means that any make that cooks this tester, *has* to
 *     	build the liboit_stubs.so module, for the _forbidden_ symbols
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
* this means that no amount of trickery will help before
* an exec, as the loader simply appends the pre-loading anyway
*/
void plUT::test_linux()
{

}

	} // namespace ut

} // namespace pl
