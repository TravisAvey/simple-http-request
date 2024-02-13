add_executable(tests tests.c)
target_link_libraries(tests httprequest)
target_compile_options(tests PRIVATE -Wall -Wextra -Werror -O -Wwrite-strings)
