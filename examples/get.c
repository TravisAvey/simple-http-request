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

  const char *headers[] = {"API-Key: asdlkfj", "key: value", "a: 1"};
  req.headers = headers;
  req.numHeaders = sizeof(headers) / sizeof(char *);

  int res = simpleHttpRequest(&req, JSON, GET);
  printf("HTTP Response code: %ld\n", req.code);
  printf("req->data: %s\n", req.body);

  simpleHttpClose();

  return 0;
}
