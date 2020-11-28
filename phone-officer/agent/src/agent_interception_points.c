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
#include "string.h"
#include "agent_interception_points.h"
#include "preload_utils.h"

static unsigned short MAX_PATH_SZ = 1024;

// the open family of functions.
// we care about opening certain files
int open(const char *pathname, int flags, ...)
{

    char buffer[MAX_PATH_SZ];
    typedef int (*open_ptr_t)(const char *pathname, int flags, ...);

    // make sure we're good, input'wise
    if(0 == pathname) return -1;
    if(MAX_PATH_SZ < get_string_length(pathname)) return -1;
    
    // TODO  maybe cache this pointer in a static?
    func_ptr_t popen = find_sym_by_name("open", PLC_NEXT, 5); 

    if(bad_func_ptr == popen) return -1;

    // my magic goes here ...
    // for now, swap the file name
    // it is assumed that the name + the addition will
    // never exceed 1024 characters   
    strcpy(buffer, pathname);
    strcat(buffer+strlen(pathname), ".preload");
 
    ((open_ptr_t)popen) (buffer, flags);

    return 0;
}

