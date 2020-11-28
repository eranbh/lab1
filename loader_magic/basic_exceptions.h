#pragma once
#include <string.h>
#include <system_error>



class filesystem_exception : public std::system_error
{

public:
    filesystem_exception() = default;
    filesystem_exception(std::string file) :
                             std::system_error{errno, std::system_category()},
                             m_file_full_name{std::move(file)}{}

private:

    std::string m_file_full_name;
};