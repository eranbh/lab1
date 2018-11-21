#pragma once
/**
 */
#include <memory>

namespace fk
{
namespace mem
{

class memory_block_view
{
public:

    memory_block_view() = default;
    memory_block_view(memory_block_view&&) = default;
    memory_block_view& operator=(const memory_block_view&) = delete;
    memory_block_view& operator=(const memory_block_view&&) = delete;

private:
    std::unique_ptr<unsigned char> m_block;
};


}
}
