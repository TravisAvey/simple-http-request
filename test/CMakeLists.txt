add_library(minunit_h INTERFACE)
target_include_directories(minunit_h INTERFACE minunit)

add_executable(tests tests.c)
target_link_libraries(tests minunit_h httprequest)
target_compile_options(
  tests
  PRIVATE -Wall
          -Wextra
          -Werror
          -O
          -Wwrite-strings
          -lrt
          -lm)
