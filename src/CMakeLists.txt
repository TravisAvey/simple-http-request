function(simplerequest)
  add_library(simple-http-request request.c)
  target_link_libraries(simple-http-request PUBLIC curl)

endfunction()
