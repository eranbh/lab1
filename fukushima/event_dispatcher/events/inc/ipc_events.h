#pragma once
#include <variant>

namespace fk
{
namespace ev 
{


using native_handle = int;



// internal definitions    
namespace bits
{
    struct ipc_event
    {

        ipc_event(native_handle handle):
            m_handler{handle}{}

        void handle(native_handle);

        native_handle m_handler;
    };
}


class socket_connect_event : private  bits::ipc_event
{
    socket_connect_event(native_handle handle): ipc_event{handle}{}
    using ipc_event::handle;
};

class socket_input_ready_event : private bits::ipc_event
{
    socket_input_ready_event(native_handle handle): ipc_event{handle}{}
    using ipc_event::handle;
};

class socket_output_ready_event : private bits::ipc_event
{
    socket_output_ready_event(native_handle handle): ipc_event{handle}{}
    using ipc_event::handle;
};

using ipc_events = 
   std::variant<socket_connect_event, socket_input_ready_event, socket_output_ready_event>;

}
}
