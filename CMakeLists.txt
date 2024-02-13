cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

project(simpleHTTPRequest VERSION 0.1.0)

# TODO: add in ability to choose which of these to build.. make will do all so
# make --test or make --examples for example

# TODO: Add in target for install and debug (will need to add in code for
# verbose printf statements?)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
