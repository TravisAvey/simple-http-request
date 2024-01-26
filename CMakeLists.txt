cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

project(simpleHTTPRequest VERSION 0.0.1)

find_package(CURL REQUIRED)
add_library(httprequest STATIC src/request.c)
target_include_directories(httprequest
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(simpleHTTPRequest test/tests.c)

target_include_directories(simpleHTTPRequest PUBLIC ${CURL_INCLUDE_DIRS})

# may need to add ${CURL_DEPENDENCIES} if required
target_link_libraries(simpleHTTPRequest PUBLIC httprequest ${CURL_LIBRARIES})
