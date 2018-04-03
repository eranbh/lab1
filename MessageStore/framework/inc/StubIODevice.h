#pragma once

#ifndef MESSAGESTORE_STUBIODEVICE_H
#define MESSAGESTORE_STUBIODEVICE_H
/*
* a stub implementation for testing  ... maybe ??
*/

#include "inc/IODevices.h"


namespace framework {

class StubIODevice : public IODevice {

    virtual std::any readData(std::uint64_t) override {return 9;}

    virtual void writeData(std::any, std::uint64_t) override {}

    virtual void
    readData(std::any, std::uint64_t, IODeviceErrors &) override {}

    virtual void
    writeData(std::any, std::uint64_t, IODeviceErrors &) override {}

    virtual void ignoreTillChar(char ch) override {}
};

}

#endif //MESSAGESTORE_STUBIODEVICE_H
