add_library(minunit_h INTERFACE)
target_include_directories(minunit_h INTERFACE minunit)

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE util)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/post.txt
               ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(tests tests.c)
target_link_libraries(tests minunit_h utils httprequest)
target_compile_options(
  tests
  PRIVATE -Wall
          -Wextra
          -Werror
          -O
          -Wwrite-strings
          -lrt
          -lm)
