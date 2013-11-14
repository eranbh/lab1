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
#include <string.h>
#include <fcntl.h> // no need for this !!!
#include "dlfcn.h"
#include "cppunit/extensions/HelperMacros.h"
#include "plUT.h" // my test cases

//extern int stamFunc();


namespace pl{
	namespace ut{

#define __GET_DL_ERROR(STM,MSG,RET)                        \
do{                                                        \
	dlerror();                                             \
	if(RET) CPPUNIT_ASSERT_MESSAGE(MSG " RET",(0 != STM)); \
	STM;                                                   \
	const char* err=dlerror();                             \
	err= (0 != err)?err:"";                                \
	printf("%s\n", err);                                   \
	char buff[1024]={0};                                   \
    memcpy(buff, MSG, strlen(MSG));                        \
	memcpy(buff+strlen(MSG)+1,err , strlen(err));          \
	CPPUNIT_ASSERT_MESSAGE(buff,(0 == strcmp(err,"")));    \
}while(0)


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
	void* pfirstOpen = 0;
	__GET_DL_ERROR((pfirstOpen=dlsym(RTLD_DEFAULT, "stamFunc")),"plUT::test_linux",0);
	__GET_DL_ERROR((pfirstOpen=dlsym(RTLD_DEFAULT, "open")),"plUT::test_linux",0);
	Dl_info dlInfo;
	open("stam", O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
	__GET_DL_ERROR((dladdr(pfirstOpen, &dlInfo)),"plUT::test_linux",1);

	printf("%s\n", dlInfo.dli_fname);
}

	} // namespace ut

} // namespace pl
