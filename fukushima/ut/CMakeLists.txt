


include_directories (PUBLIC ${3RD_PARTIES_ROOT}/gtest/include
                            ${PROACTOR_INCLUDE_DIRS})

link_directories ( ${3RD_PARTIES_ROOT}/gtest/build)

set ( UT_SRC ${FUKUSHIMA_ROOT}/ut/src/test_proactor.cpp)

add_executable (test_fukushima ${UT_SRC})

target_link_libraries (test_fukushima gtest_main gtest proactor pthread )

set_target_properties(test_fukushima PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
