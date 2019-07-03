#pragma once
#include <string>
/**
*/
namespace cyrus
{
namespace loader
{
    using object_handle = void*;

    class loadable_object
    {
        public:

            loadable_object(std::string object_path);
            ~loadable_object();
            loadable_object(const loadable_object&);
            loadable_object& operator=(const loadable_object&);
            loadable_object(loadable_object&&);
            loadable_object& operator=(loadable_object&&);
        
        private:
            
            object_handle m_object_handle;
            std::string m_object_path{""};
    };
}
}
