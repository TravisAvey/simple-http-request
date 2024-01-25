cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

project(simpleHTTPrequest)

find_package(CURL REQUIRED)

set(HEADERS "include/request.h")
set(SOURCES "src/request.c")

target_link_libraries(simpleHTTPrequest PUBLIC CURL::libcurl)

# add_subdirectory(src)
