function(add_example_exe name)
  add_executable(${name} ${name}.c)
  target_link_libraries(${name} httprequest)
  target_compile_options(${name} PRIVATE -Wall -Wextra -Werror -O
                                         -Wwrite-strings)
endfunction()

add_example_exe(get)
add_example_exe(post)
add_example_exe(put)
add_example_exe(patch)
add_example_exe(delete)
