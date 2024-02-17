// include the request.h file
// compile with the libhttprequest.a file
// and with libcurl
#include "../include/request.h"

int main() {
  /*
   * Overview of calling a POST request:
   *   - Initialize library with a request object
   *   - Set a URL
   *   - Set any text (CSV, JSON, HTML, TEXT, XML)
   *   - Call the request (simpleHttpRequest)
   *   - Check for any errors
   *   - check response code and body
   *   - close the library
   */

  // Initialize a request and response object
  // the request object will hold information
  // for sending an HTTP request
  //
  // The response object will hold response
  // information, such as a response code (200, 400, etc)
  // and the response body--the text/json/etc returned from
  // the server
  request req;
  response res;

  // initialize the library with a reference to the request object
  simpleHttpInit(&req);

  // set the URL for the request
  req.url = "https://httpbin.org/post";
  // set text to be sent to the server
  // this example is JSON
  req.text = "{ \"name\": \"Jim Halpert\"}";

  // send the request:
  // reference to the request and response objects
  // JSON enum, this example is sending JSON
  // POST enum, we are sending a POST request
  error err = simpleHttpRequest(&req, &res, JSON, POST);

  // check the errors
  // note: these will be library specific errors
  // any server error will be in the response code/body
  if (err != NO_ERROR)
    printf("error: %s\n", simpleHttpErrorString(err));

  // check the response code and body
  printf("Response Code: %ld\n", res.code);
  printf("Response: %s\n", res.body);

  // be sure to close the library with references
  // to the request and response objects
  simpleHttpClose(&req, &res);

  return 0;
}
