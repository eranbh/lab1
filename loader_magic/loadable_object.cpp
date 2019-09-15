
#include "loadable_object.h"



namespace loader
{

loadable_object::loadable_object(std::string object_path) :
    m_object_handle{std::move(object_path)}
{}

loadable_object::~loadable_object()
{}

loadable_object&
loadable_object::operator=(const loadable_object& a)
{
    return const_cast<loadable_object&>(a);
}

loadable_object&
loadable_object::operator=(loadable_object&& a)
{
    return const_cast<loadable_object&>(a);
}


}

