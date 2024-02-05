/*
 *   Show simple example of a GET request
 *
 *
 */

#include "../include/request.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  request req;
  simpleHttpInit();
  req.url = "https://catfact.ninja/fact";
  int res = simpleHttpGet(&req);
  printf("HTTP Response code: %ld\n", req.httpCode);
  printf("req->data: %s\n", req.responseText);
  simpleHttpClose();

  return 0;
}
