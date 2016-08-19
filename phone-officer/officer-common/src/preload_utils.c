/*
* this module contains internal preloading facilities
* at this moment I dont see a need to expose the syms
* in this module, so no h file will be provided for it.
* this module is aware of pre-loading, it is not a 
* general purpose dl-api wrapper. use it wisely
*/
#include <stdarg.h> // var args handling
#include "preload_utils.h" // pre-loading exposed definitions

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


    switch(lookup_policy)
    {
        case PLC_LIB_NM:
        {
            // a lib name should have been sent
           va_list ap;
           va_start(ap, "%s");
           char* plibNm = va_arg(ap, char*);
           va_end(ap);
        } 
        case PLC_NEXT:
        default: break; // make compiler happy 
    }

    return bad_func_ptr; 
}
