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
#ifndef INC_AGENT_INTERCEPTION_POINTS_H_
#define INC_AGENT_INTERCEPTION_POINTS_H_
#include <sys/stat.h> // for mode_t

	// the open family of functions.
	// we care about opening certain files

        int open(const char *pathname, int flags, ...);

        int creat(const char *pathname, mode_t mode);

        int openat(int dirfd, const char *pathname, int flags, ... );	


#endif /* INC_AGENT_INTERCEPTION_POINTS_H_ */
