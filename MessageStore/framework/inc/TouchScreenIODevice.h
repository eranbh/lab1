#pragma once
/*
* this header would, theoretically hold the implementation
* of the IODevices' services for a touch screen device.
* any specifics concerning:
* 1. init'ing / tearing down special HW would be done here.
* 2. additional error handling, relevant to this layer alone.
*/
#include "inc/IODevices.h"

#ifndef MESSAGESTORE_TOUCHSCREENIODEVICE_H
#define MESSAGESTORE_TOUCHSCREENIODEVICE_H

namespace framework {

// just an example: we chose to impl only the non-throwing
// part of the IODevice interface. in this case, there should be
// concrete IODeviceErrors defined here, to cover erroneous cases
class TouchScreenIODevice : public IODevices {
    virtual void
    readData(std::any, std::uint64_t, IODeviceErrors &) override;

    virtual void
    writeData(std::any, std::uint64_t, IODeviceErrors &) override;

    // ... many other stuff that are needed
};

}

#endif //MESSAGESTORE_TOUCHSCREENIODEVICE_H
