find_package(CURL REQUIRED)

add_library(httprequest STATIC request.c)

target_link_libraries(httprequest PUBLIC header curl)

add_library(header INTERFACE)
target_include_directories(header INTERFACE ${CMAKE_SOURCE_DIR}/include)
