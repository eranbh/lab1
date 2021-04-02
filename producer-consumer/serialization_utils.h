#pragma once

#include <unistd.h>
#include <vector>
#include <iostream>

template<typename ToSerializeType>
void writeToSocket(int socket, ToSerializeType msg)
{
    auto buff =  reinterpret_cast<char*>(&msg);
    if(int amount = ::write(socket, buff, sizeof(ToSerializeType)) ; amount == -1)
    {
        std::cout << "write failed. " << errno << std::endl;
    }
}

template<typename FromSerializeType>
FromSerializeType readFromSocket(int socket)
{
    FromSerializeType msg{};
    auto buff =  reinterpret_cast<char*>(&msg);

    if(int amount = ::read(socket, buff, sizeof(FromSerializeType)) ; amount == -1)
    {
        std::cout << "read failed. " << errno << std::endl;
    }

    return msg;
}