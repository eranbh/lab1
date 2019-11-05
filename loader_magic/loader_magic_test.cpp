#include "gtest/gtest.h"
#include "loadable_object.h"


TEST(loader_magic, test_ctor)
{
    loader::loadable_object lo{"/usr/lib/libgjs.so.0"};
}