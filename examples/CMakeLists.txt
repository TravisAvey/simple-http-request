# add_executable(get get.c) add_executable(post post.c) add_executable(delete
# delete.c) add_executable(put put.c) add_executable(patch patch.c)
# target_link_libraries(get httprequest) target_link_libraries(post httprequest)
# target_link_libraries(delete httprequest) target_link_libraries(put
# httprequest) target_link_libraries(patch httprequest)

function(add_example_exe name)
  add_executable(${name} ${name}.c)
  target_link_libraries(${name} httprequest)
endfunction()

add_example_exe(get)
add_example_exe(post)
add_example_exe(put)
add_example_exe(patch)
add_example_exe(delete)
