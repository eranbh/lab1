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


extern char **environ;

const char* const LIBC_NAME = "libc.so.6";


// returns the length of the string sent in.
// NOTE: if the str is not null terminated, the 
// behavior of this function UNDEFINED
// also, we made this symbol static, as we only want
// this to be used internally. this does not make any
// input validations
inline 
static int get_string_length(const char* const str)
{
    int len = 0;
   
    for( ; str[len] ; len++){}

    return len;
}


// this function will add the variable sent to it, to
// the users environment. the idea here is that we can
// not use any libc facility for this, as we might be preloaded
// there is an assumption here:
// the parameter sent in, is a well formated:
// "key=value\0" pair. the result of it being something else
// could prove to be catastrophical. especialy if it is not
// properly null terminated
// this function _always_ allocates the extra memory needed
// for the new env. there is no checking for duplications
// we try _very hard_ not to use libc syms here. this code
// might be called from preloaded code
int insert_entry_to_env(const char* const entry)
{
    int envLen = 0;
    char** pNewEnv = 0;
    
    typedef void* (*malloc_ptr_t)(size_t);
    malloc_ptr_t pmalloc = 0; 
    
    typedef char* (*strdup_ptr_t)(const char*);
    strdup_ptr_t pstrdup = 
       (strdup_ptr_t) find_sym_by_name("strdup", PLC_LIB_NM, LIBC_NAME);


    //TODO the case for environ == 0 needs more care
    if(0 == environ) return -1;


     __SYS_CALL_TEST_NN_RETURN_VAL((malloc_ptr_t)find_sym_by_name("malloc", 
                                                                  PLC_LIB_NM, LIBC_NAME), 
                                   -1);

    // dont loose the handle to the old env [backup]
    char* pOldEnv = *environ;
    
    // take the length of the current env
    for( ; environ[envLen] ; envLen++);

    // allocate the new array of pointers. we need extra slot for null
    __SYS_CALL_TEST_NN_RETURN_VAL(pNewEnv = pmalloc((envLen+2)*sizeof(char*)), -1);

   // copy env
   for(envLen=0 ; environ[envLen] ; envLen++)
   {
       char* envVar = 0;
       
       __SYS_CALL_TEST_NN_RETURN_VAL(envVar=pstrdup(environ[envLen]), -1);

      pNewEnv[envLen] = envVar; 
   }

   // add new env var 
   pNewEnv[envLen] = pstrdup(entry);

   // add null termination
   pNewEnv[envLen] = 0;

   // point to the new array
   environ = pNewEnv; // new env in place

   // set the old memory free
   free(pOldEnv);

   return 0;
}

// this is a substitute function for the
// getenv function. you need this if you dont want
// to access libc from preloaded code [as i do]
const char* find_entry_in_env(const char* const entry_key)
{
    typedef int (*memcmp_ptr_t) (void*, const void*, size_t );
    memcmp_ptr_t pmemcmp =
           (memcmp_ptr_t) find_sym_by_name("memcmp", PLC_LIB_NM, LIBC_NAME);

    int entSz = get_string_length(entry_key);


    size_t i;
    for(i=0; environ[i] ; i++)
    {
        if(entSz == get_string_length(environ[i]))
        {
            if(0 == pmemcmp(environ[i], entry_key, entSz))
            {
                // we assume it a valid entry: [key=value]
                return environ[i]+entSz+1;
            }
        } 
    }
    return 0;
}

// this function should be used to push new values
// to existing env variables.
// the arguments are assumed to be null terminated.
// this function scans the env for the given key
// if that isnt found, it will fall back on a full 
// insert_entry_to_env which _will_ reallocate the env.
int update_entry_in_env(const char* const entry_key, 
                        const char*       entry_value)
{
    const char* pCurrEntry=0;
    char*       pNewEntry=0;
    int entryLen=0;

    typedef void* (*malloc_ptr_t)(size_t);
    malloc_ptr_t pmalloc=0; 
    
    // first see if this entry exists
    if(0 == (pCurrEntry=find_entry_in_env(entry_key)))
    {
        // the entry does not exist. lets add it
        __SYS_CALL_TEST_NM1_RETURN(insert_entry_to_env(entry_key));
       return 0;
    }

    // the entry was found. lets build an entry and place it in env
    __SYS_CALL_TEST_NN_RETURN_VAL(pmalloc=(malloc_ptr_t)find_sym_by_name("malloc", 
                                                                         PLC_LIB_NM, LIBC_NAME),
                                           -1);
    
    // we need room for: null + equal sign
    entryLen = get_string_length(entry_key) + 
               get_string_length(entry_value) +
               2;
   
    __SYS_CALL_TEST_NN_RETURN_VAL(pNewEntry=pmalloc(entryLen), -1);

   // copy data to new entry. format: <key=value>
   size_t i;
   for(i=0; entry_key+i ; pNewEntry++){*pNewEntry=entry_key[i];}
   *pNewEntry='=';
   for(i=0; entry_value+i ;  pNewEntry++){*pNewEntry=entry_value[i];}
   *pNewEntry='\0';

   // add the new entry to the current env
   // in order to do that, we need the _actual_
   // address for this entry in environ
   for(i=0;environ[i] != pCurrEntry;i++){}
   environ[i]=pNewEntry;

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


