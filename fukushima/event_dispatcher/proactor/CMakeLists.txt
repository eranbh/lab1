# this builds 



set ( PROACTOR_INCLUDE_DIRS PUBLIC   ${FUKUSHIMA_ROOT}/event_dispatcher/proactor/inc)

add_library (proactor src/proactor.cpp)

target_include_directories (proactor PUBLIC ${FUKUSHIMA_ROOT}/event_dispatcher  ${PROACTOR_INCLUDE_DIRS})

set_target_properties(proactor PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
