
/*
 *   Show simple example of a DELETE request
 *
 *
 */

#include "../include/request.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  request req;
  simpleHttpInit();
  req.url = "https://httpbin.org/delete";
  int res = simpleHttpRequest(&req, JSON, DELETE);
  printf("HTTP Response code: %ld\n", req.code);
  printf("req->data: %s\n", req.body);
  simpleHttpClose();

  return 0;
}
