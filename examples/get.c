/*
 *   Show simple example of a GET request
 *
 *
 */

#include "../include/request.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  request req;
  simpleHttpInit(&req);

  req.url = "https://catfact.ninja/fact";
  // req.url = "https://postman-echo.com/digest-auth";

  const char *headers[] = {"API-Key: asdlkfj", "key: value", "a: 1"};
  req.headers = headers;
  req.numHeaders = sizeof(headers) / sizeof(char *);

  int res = simpleHttpRequest(&req, JSON, GET);
  printf("HTTP Response code: %ld\n", req.code);
  printf("req->data: %s\n", req.body);

  simpleHttpClose(&req);

  return 0;
}
