cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

project(simpleHTTPRequest VERSION 1.0.0)

# Add debugging option
option(VERBOSE "Enter debug mode" OFF)
if(VERBOSE)
  add_definitions(-DVERBOSE)
endif()

set(TARGET_GROUP
    production
    CACHE STRING "What are we building?")
add_subdirectory(src)
if(TARGET_GROUP STREQUAL release)
  # not needing to add anything for a release build yet
elseif(TARGET_GROUP STREQUAL test)
  add_subdirectory(test)
elseif(TARGET_GROUP STREQUAL examples)
  add_subdirectory(examples)
else()
  message(FATAL_ERROR "We don't have instructions to build ${TARGET_GROUP}!")
endif()
