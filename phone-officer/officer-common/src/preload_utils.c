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


void insert_pre_loading_to_env(const char* const libPath)
{
    
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
