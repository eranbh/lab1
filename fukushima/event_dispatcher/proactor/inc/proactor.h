#pragma once

#include "proactor.h"
#include "events/inc/ipc_events.h"

namespace fk
{
namespace evt
{


class proactor
{
public:
    void start_event_loop();
    void stop_event_loop();

    template<typename EventType>
    void register_event_handler(EventType);
};


}
}
