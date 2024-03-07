find_package(CURL REQUIRED)

add_library(SimpleHttpRequest STATIC request.c)

target_link_libraries(SimpleHttpRequest PUBLIC header curl)

add_library(header INTERFACE)
target_include_directories(header INTERFACE ${CMAKE_SOURCE_DIR}/include)

target_compile_options(SimpleHttpRequest PRIVATE -Wall -Wextra -Werror -O
                                                 -Wwrite-strings)
