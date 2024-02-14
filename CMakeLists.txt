cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

project(simpleHTTPRequest VERSION 0.1.0)

# Add debugging option
option(VERBOSE "Enter debug mode" OFF)
if(VERBOSE)
  add_definitions(-DVERBOSE)
endif()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
