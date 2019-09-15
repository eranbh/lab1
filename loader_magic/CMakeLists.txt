
link_libraries(-ldl)

# builds the loader magic library
add_library(loader_magic SHARED loadable_object.cpp)

# builds a testing suite

# pull in the gtest stuff
link_directories(/usr/src/gtest/build)
link_libraries(-lgtest -lgtest_main)

#link with the loader magic lib
link_directories(/home/eranb/work/lab1/loader_magic/build)
link_libraries(-lloader_magic)

add_executable(loader_magic_test loader_magic_test.cpp)