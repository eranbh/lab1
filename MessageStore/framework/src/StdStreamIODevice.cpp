//
// Created by Eran Ben Horin on 29/03/2018.
//
#include <algorithm>
#include <iterator>
#include <limits>
#include "inc/StdStreamIODevice.h"

namespace framework {

std::any
StdStreamIODevice::readData(std::uint64_t len)
{
    if(!m_inStream.good()){ throw StdStreamException{};}

    StreamDataChunk dataChunk;
    dataChunk.reserve(len);
    // working directly with an istrean is prety low
    // on abstractions. we have to read the char one by one
    // note that we preserve the new line character!
    char ch = 0;
    for( std::uint64_t i=0 ; i<len && (ch != '\n') ; ++i)
    {
        m_inStream >> ch;
        dataChunk.push_back(ch);
    }

    return dataChunk;
}

void StdStreamIODevice::writeData(std::any buff, std::uint64_t len)
{
    if(!m_outStream.good()){ throw StdStreamException{};}

    StreamDataChunk dataChunk = std::any_cast<StreamDataChunk&&>(buff);
    std::copy(dataChunk.begin(), dataChunk.end(),
              std::ostream_iterator<char>(m_outStream));
    m_outStream << std::endl;
}

void StdStreamIODevice::ignoreTillChar(char ch)
{
    m_inStream.ignore(std::numeric_limits<std::streamsize>::max(), ch);
}

} // namespace framework

