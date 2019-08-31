#include <dlfcn.h>
#include "loadable_object.h"



namespace loader
{

loadable_object::loadable_object(std::string object_path) :
    m_object_path{std::move(object_path)}
{
    m_object_handle = dlopen("lib.so", RTLD_LAZY);
    if(NULL == m_object_handle){
        throw "";
    }
}

loadable_object::~loadable_object()
{}

loadable_object::loadable_object(const loadable_object&)
{}

loadable_object&
loadable_object::operator=(const loadable_object& a)
{
    return const_cast<loadable_object&>(a);
}

loadable_object::loadable_object(loadable_object&&)
{}

loadable_object&
loadable_object::operator=(loadable_object&& a)
{
    return const_cast<loadable_object&>(a);
}


}

