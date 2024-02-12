
/*
 *   Show simple example of a DELETE request
 *
 *
 */

#include "../include/request.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  request req;
  response res;
  simpleHttpInit(&req);
  req.url = "https://httpbin.org/delete";
  error err = simpleHttpRequest(&req, &res, JSON, DELETE);
  printf("HTTP Response code: %ld\n", res.code);
  printf("req->data: %s\n", res.body);
  simpleHttpClose(&req, &res);

  return 0;
}
