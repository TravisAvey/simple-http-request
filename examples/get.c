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
  req.url = "https://postman-echo.com/digest-auth";
  // req.url = "https://a0bcf57c-08cf-4ecf-93a7-768b7e2f60d9.mock.pstmn.io/get";

  const char *headers[] = {"Authorization: Basic cG9zdG1hbjpwYXNzd29yZA=="};
  req.headers = headers;
  req.numHeaders = sizeof(headers) / sizeof(char *);

  simpleHttpSetPassword(&req, "postman:password", DIGEST);

  int res = simpleHttpRequest(&req, JSON, GET);
  printf("HTTP Response code: %ld\n", req.code);
  printf("req->data: %s\n", req.body);

  simpleHttpClose(&req);

  return 0;
}
