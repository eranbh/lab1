/*
 * agent_interception_points.h
 *
 *  Created on: May 19, 2016
 *      Author: eranbh
 *
 * this is the file holding the main interception
 * points for the agent module. keep this file simple
 * and always abi compatible with C
 *
 */

#include "agent_interception_points.h"
#include "stdio.h"

	// the open family of functions.
	// we care about opening certain files

	int open(const char *pathname, int flags)
	{printf("myopen"); return 0;}

	int open(const char *pathname, int flags, mode_t mode)
	{return 0;}

	int creat(const char *pathname, mode_t mode)
	{return 0;}

	int openat(int dirfd, const char *pathname, int flags)
	{return 0;}

	int openat(int dirfd, const char *pathname, int flags, mode_t mode)
	{return 0;}

