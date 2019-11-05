
#include "loadable_object.h"



namespace loader
{


template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;


loadable_object::loadable_object(std::string object_path)
{
    loader_result result = loader::open(std::move(object_path));
    
    //TODO implement visitation - this can throw
    std::visit(
        overload{
            [](loader_exception&){
                // this can be another variant of errors
            },
            [this](loader_fd& fd){
                m_object_handle = std::move(fd);
            }
        },
        result);
}


}

