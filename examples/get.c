#include "../include/request.h"
#include <stdio.h>

int main() {
  /*
   * Overview of calling a GET request:
   *   - Initialize library with a request object
   *   - Set a URL
   *   - add optional headers or username/password
   *   - Call the request (simpleHttpRequest)
   *   - Check for any errors
   *   - check response code and body
   *   - close the library
   */

  // declare the request object
  request req;
  response res;
  // initialize the simple http library
  simpleHttpInit(&req);

  // set the url where the request is going
  req.url = "https://catfact.ninja/fact";

  // call the request:
  // pass in the request object
  // the media type (for headers)
  // And the request type
  error err = simpleHttpRequest(&req, &res, JSON, GET);
  // check for errors; 0 or NO_ERROR is success
  if (res.err != NO_ERROR) {
    printf("Error: %s\n", simpleHttpErrorString(res.err));
  }
  // Check the response:
  printf("HTTP Response code: %ld\n", res.code);
  printf("req->data: %s\n", res.body);

  // Another example that requires auth:
  // set the url
  req.url = "https://postman-echo.com/digest-auth";

  // Depending on the server requirements set the headers:
  // Add more to the array as needed, each will be added to the request headers
  //
  // Headers can be added to any request
  const char *headers[] = {"Authorization: Basic cG9zdG1hbjpwYXNzd29yZA=="};
  req.headers = headers;
  // set the number of headers your adding
  req.numHeaders = sizeof(headers) / sizeof(char *);

  // set the username and password
  // pass in the request object
  // the username and password as "username:password"
  // If the server requires DIGETS, otherwise NONE
  simpleHttpSetPassword(&req, "postman:password", DIGEST);

  // call the request
  simpleHttpRequest(&req, &res, JSON, GET);
  // check for errors
  if (res.err != NO_ERROR) {
    printf("Error: %s\n", simpleHttpErrorString(res.err));
  }
  // check response code and body
  printf("HTTP Response code: %ld\n", res.code);
  printf("req->data: %s\n", res.body);

  // when done: close the library
  simpleHttpClose(&req, &res);

  return 0;
}
