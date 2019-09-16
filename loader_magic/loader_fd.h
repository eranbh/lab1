#pragma once
#include <dlfcn.h>
#include <string>
#include "basic_exceptions.h"

namespace loader
{


struct loader_exception : std::system_error
{
    
};    

// the loader ctor logic
class loader_fd
{
public:

    using raw_handle = void*;

    loader_fd(std::string path){

        m_raw_handle = dlopen("lib.so", RTLD_LAZY);
        if(NULL == m_raw_handle){
            throw "";
        }
    }

    ~loader_fd(){
        dlclose(m_raw_handle);
    }

    raw_handle m_raw_handle;
};

} // namespace loader