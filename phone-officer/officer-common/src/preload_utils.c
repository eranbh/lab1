/*
* this module contains internal preloading facilities
* at this moment I dont see a need to expose the syms
* in this module, so no h file will be provided for it.
* this module is aware of pre-loading, it is not a 
* general purpose dl-api wrapper. use it wisely
*/
#define _GNU_SOURCE
#include <dlfcn.h> // for dl apis
#include <stdarg.h> // var args handling
#include "macros.h" // our macros
#include "preload_utils.h" // pre-loading exposed definitions



const char* const LIBC_NAME = "libc.so.6"

// this function will add the variable sent to it, to
// the users environment. the idea here is that we can
// not use any libc facility for this, as we might be preloaded
// there is an assumption here:
// the parameter sent in, is a well formated:
// "key=value\0" pair. the result of it being something else
// could prove to be catastrophical. especialy if it is not
// properly null terminated
// this function _always_ allocates the extra memory needed
// for the new pair. there is no checking for duplications
int insert_entry_to_env(const char* const entry)
{
    int envLen = 0;
    char* pNewEnv = 0;
    
    // no point in searching for the sym more than once
    static void* (*pmalloc)(size_t) = 
        find_sym_by_name("malloc", PLC_LIB_NM, LIBC_NAME);

    if(0 == pmalloc || 0 == environ) return -1;

    // dont loose the handle to the old env [backup]
    const char* pOldEnv = *environ;
    
    // take the length of the current env
    for(envLen ; environ[envLen] ; envLen++);

    // allocate the new array. we need extra slot for null
    __SYS_CALL_TEST_NN_RETURN_VAL(pNewEnv = pmalloc(envLen+1), -1);

   // copy env
   for(envLen=0 ; environ[envLen] ; pNewEnv[envLen] = environ[envLen], envLen++);

   // add null termination
   pNewEnv[envLen] = 0;

   // point to the new array
   *environ = pNewEnv; // new env in place

   // set the old memory free
   free(pOldEnv);

   return 0;
}




func_ptr_t find_sym_by_name(const char* sym_name, 
                            sym_lookup_policy_t lookup_policy, 
                            ...)
{
    if(0 >= sym_name || PLC_INVLD <= lookup_policy )
    {
         // in any other circumstance this would have
         // been an assert. but we cant fail the calling
         // process, as we are only guests
         return bad_func_ptr; 
    }

    void* phandle = RTLD_NEXT,* psym = 0;
    switch(lookup_policy)
    {
        case PLC_LIB_NM:
        {
           // a lib name should have been sent
           va_list ap;
           va_start(ap, lookup_policy);
           char* plibNm = va_arg(ap, char*);
           if(0 >= plibNm) return bad_func_ptr;
           va_end(ap);
           __SYS_CALL_TEST_NN_CAST_RETURN(phandle=dlopen(plibNm, RTLD_LAZY), 
                                          func_ptr_t);
           // fall through here !!!
        } 
        case PLC_NEXT:
        {
             __SYS_CALL_TEST_NN_CAST_RETURN(psym=dlsym(phandle, sym_name),
                                            func_ptr_t);
        }
        default: break; // make compiler happy 
    }

    return (func_ptr_t)psym; 
}
