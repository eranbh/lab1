//
// Created by Eran Ben Horin on 29/03/2018.
//

#ifndef MESSAGESTORE_STDSTREAMIODEVICE_H
#define MESSAGESTORE_STDSTREAMIODEVICE_H

#include <iostream>
#include <vector>
#include "IODevices.h"

namespace framework{

   using StreamDataChunk = std::vector<char>;
/*
* this class is a concrete impl of a device for std streams.
* this is a TOY impl! the streams in the std-lib are neither
* copyable, or movable.
* reading this device is line-oriented: this means that reading
* skips any whitespacing it might come across, but terminate on
* new line characters.
* no error handling is done, except very naiive stuff.
* not all of the IODevice apis were handled here.
* the code relies on the basic operator the std-lib has:
* operator<</>>
* */
class StdStreamIODevice : public IODevice {

public:

    StdStreamIODevice(std::istream& in, std::ostream& out) :
          m_inStream(in), m_outStream(out){m_inStream >> std::noskipws;}

    // reads __up to__ the requested amount of bytes.
    // if a new line is encountered first, it terminates.
    // this line buffering is to support terminals as std-ins
    virtual std::any readData(std::uint64_t) override ;

    // writes the content of the buffer into the device.
    // will always write StreamDataChunk of length len
    virtual void writeData(std::any, std::uint64_t) override ;

    // ignores chars up to a given delimiter character.
    // this is useful when you read less then
    // a given len. giving offset semantics to streams
    virtual void ignoreTillChar(char ch) override ;


    struct StdStreamException{
        // .. reason, error code, whatever!!
    };

    // ... many other things you might want from a stream

private:
    std::istream& m_inStream;
    std::ostream& m_outStream;
};

    // TODO add free standing functions that will give the user
    // code that stream-ish look and feel [ operator << / >> etc ]

}


#endif //MESSAGESTORE_STDSTREAMIODEVICE_H
