# this builds fukushima products
cmake_minimum_required(VERSION 3.10)

# this builds compilation database
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set ( PROJECT_NAME fukushima )

project (${PROJECT_NAME})

set (FUKUSHIMA_ROOT $ENV{LAB}/fukushima)
# this is temp ... until i figure out the way
# to get access to github repos from thius machine
set (3RD_PARTIES_ROOT PUBLIC /home/eranb/work/lab1/3rd_parties)

# building the receipies for the rest of the project
add_subdirectory (event_dispatcher)
add_subdirectory (ut)
