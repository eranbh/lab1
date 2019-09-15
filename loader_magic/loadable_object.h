#pragma once
#include <string>
#include "loader_fd.h"
/**
*/

namespace loader
{
    class loadable_object
    {
        public:

            loadable_object(std::string object_path);
            ~loadable_object();
            loadable_object(const loadable_object&);
            loadable_object& operator=(const loadable_object&);
            loadable_object(loadable_object&&);
            loadable_object& operator=(loadable_object&&);

            void proxy_object(std::string);

            void inject_to_process(std::string);

            // ...
        
        private:
            
            loader_fd m_object_handle;
    };
}
