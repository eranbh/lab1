#pragma once
#include <dlfcn.h>
#include <variant>
#include "basic_exceptions.h"


namespace loader
{


struct loader_exception : std::system_error 
{
    std::string m_libName;
    
public:
    loader_exception() = default;
     loader_exception (std::string libName) : 
     std::system_error {errno,
              std::system_category(),
              strerror(errno)},
         m_libName{std::move(libName)}{}
      loader_exception& operator=(loader_exception) {}   
};

/**
 * this is a guard type class that wraps a shared object's handle.
 * this is an RAII handle, and therefor must __never__ throw!!
*/
class loader_fd
{
public:
    using raw_handle = void*;
    
    loader_fd() = default;
    loader_fd& operator=(const loader_fd&) = delete;

    friend std::variant<loader_fd, loader_exception> open(std::string path);

     loader_fd(std::string path) noexcept{

         m_raw_handle = dlopen(path.data(), RTLD_LAZY);
         if(NULL == m_raw_handle){
             // report
         }
     }

     loader_fd(loader_fd&& other){
         m_raw_handle = other.m_raw_handle;
         other.m_raw_handle = NULL;
     }

     loader_fd& operator=(loader_fd&& other) {
         m_raw_handle = other.m_raw_handle;
         other.m_raw_handle = NULL;
     }

     //TODO impl other special functions

    ~loader_fd() {
        if(m_raw_handle){
            dlclose(m_raw_handle);
        }
    }

 private:

    raw_handle get_fd() const{
        return m_raw_handle;
    }

    raw_handle m_raw_handle{NULL};
};

using loader_result = std::variant<loader_fd, loader_exception>;

loader_result
 open(std::string path)
{
    loader_fd fd{path}; // dont move from path just yet
    if(NULL == fd.get_fd()){
        return loader_result{loader_exception{std::move(path)}};
    }
    return loader_result{std::move(fd)};
}

} // namespace loader