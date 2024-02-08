#include "../include/request.h"
#include <stdio.h>

int main() {
  /*
   * Overview of calling a GET request:
   *   - Initialize library with a request object
   *   - Set a URL
   *   - Call the request (simpleHttpRequest)
   *   - Check for any errors
   *   - check response code and body
   *   - close the library
   */

  // declare the request object
  request req;
  // initialize the simple http library
  simpleHttpInit(&req);

  // set the url where the request is going
  req.url = "https://catfact.ninja/fact";

  // call the request:
  // pass in the request object
  // the media type (for headers)
  // And the request type
  int res = simpleHttpRequest(&req, JSON, GET);
  // check for errors; 0 or NO_ERROR is success
  if (res != NO_ERROR) {
    printf("Error: %s\n", simpleHttpErrorString(res));
  }
  // Check the response:
  printf("HTTP Response code: %ld\n", req.code);
  printf("req->data: %s\n", req.body);

  // Another example that requires auth:
  // set the url
  req.url = "https://postman-echo.com/digest-auth";

  // Depending on the server requirements set the headers:
  // Add more to the array as needed, each will be added to the request headers
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
  res = simpleHttpRequest(&req, JSON, GET);
  // check for errors
  if (res != NO_ERROR) {
    printf("Error: %s\n", simpleHttpErrorString(res));
  }
  printf("HTTP Response code: %ld\n", req.code);
  printf("req->data: %s\n", req.body);

  // when done, be sure to close the library
  simpleHttpClose(&req);

  return 0;
}
