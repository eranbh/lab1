/*
 * liboit_stubs.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: eran
 *  This is the module that will be used by oit tester for pl
 *  assertions. each of these symbols masks one of oit's symbols
 *  and asserts that pl did [indeed] take place
 *
 *  pl is not always welcomed, and is - sometimes - harmful.
 *  this tester asserts the execution of a sym, but in order to
 *  add the notion of right and wrong, i needed to forward the
 *  expected result on to it, in a sort of a parameterized way.
 *  I chose to do it via a file on disk:
 *  1. pl is needed - a file is created, assertion succeeds
 *  2. pl not needed - no file is created, assertion fails
 */
#include <stdio.h>

/* code inter-position starts here */

int
open(const char *pathname, int flags)
{
	printf("<<< open >>> \n");

	return 0;
}






