# Simple HTTP Request

## Description
This is a simple HTTP request library that will perform simple HTTP requests:

```
GET, POST, PUT, PATCH, DELETE
```

Currently, this library is really only set up for text on `POST, PUT, PATCH` requests--I do plan on updating to upload other media in the future.

### Motivation
I decided to make this library, OK it's nothing more than a wrapper around libCURL, anyways... I made this because I have other project ideas in mind that would make use of libCURL but wanted a simpler interface.

## Dependencies
Ensure you have libcurl installed on your system.  You may need cmake if building from source.

## Getting Started
Go to the [releases](https://github.com/travisavey/simple-http-request/releases) page
and download the latest release. libSimpleHttpRequest.a is a static C library that you can compile with your own code along with request.h

Or, you can build the library yourself, by first cloning this repo then:
```bash
cd simple-http-request
mkdir build
cd build
cmake -DTARGET_GROUP:STRING={release,test,examples} ..
make
```
`release` will build only the static library, `test` will build the tests, and `examples` will build only the examples.


You can also pass `-DVERBOSE=on` for debugging purposes (it will print a lot of information). This will build a library `libSimpleHttpRequest.a` located in build/src.


### Example
```c
#include "request.h"
#include <stdio.h>

int main() {
  // declare the request and response objects, used to store data
  request req;
  response res;

  // initialize the simple http library
  simpleHttpInit(&req);

  // set the url where the request is going
  req.url = "https://catfact.ninja/fact";

  // call the request:
  // pass in the request object
  // the media type
  // And the request type
  error err = simpleHttpRequest(&req, &res, JSON, GET);
  // check for errors
  if (err != NO_ERROR) {
    printf("Error: %s\n", simpleHttpErrorString(err));
  }

  // Check the response:
  printf("HTTP Response code: %ld\n", res.code);
  printf("HTTP Response body: %s\n", res.body);

  // when done: close the library
  simpleHttpClose(&req, &res);

  return 0;
}
```
Save your the c file and compile:
```bash
$ gcc get.c -o get -L. -lSimpleHttpRequest -lcurl
```
Then run and you should get something similar:
```bash
$ ./get
HTTP Response code: 200
HTPP Response body: {"fact":"A steady diet of dog food may cause blindness in your cat - it lacks taurine.","length":77}
```
### Documentation 
For more examples, check out the examples in the repo as these should answer most questions you might have.

All functions that you should be calling start with `simpleHttp` -- other functions are for internal use that the library will call.

If you peek inside the request.h file, you will see what each of these are:
```c
typedef struct request {
  CURL *curl;           // The CURL object
  const char *url;      // The URL; where to send the request
  const char *text;     // The text data to be sent
  const char **headers; // An array of custom headers
  int numHeaders;       // The number of custom headers
} request;

typedef struct response {
  long code;  // The response code (200, 400, 500, etc)
  char *body; // The response body text
} response;
```
The request object is what you will use to set the URL, any data to be sent, and any custom headers. 
The response object is what you will process upon the server's response: the body and code.

The different kinds of media types I used for this library are (may add more in the future):
```c
typedef enum {
  CSV,
  JSON,
  HTML,
  TEXT,
  XML
} mediaType;
```

The different errors to check for (note that these errors are the library's and not server errors--check the `response.code` and `response.body`):
```c
typedef enum {
  NO_ERROR = 0,       // success
  INIT_FAILED = 1,    // The initialization of the library failed
  NO_URL = 2,         // Missing URL for the request
  NO_MEMORY = 3,      // Couldn't initialize due to not enough memory
  REQUEST_FAILED = 4, // The request failed -- not a server error
  MEMORY_ERROR = 5    // Error initializing an object, possible memory leak
} error;
```

When adding custom headers that may be necessary for accessing server endpoints:
```c
// Headers can be added to any request
const char *headers[] = {"Authorization: Basic cG9zdG1hbjpwYXNzd29yZA=="};
req.headers = headers;

// set the number of headers your adding
req.numHeaders = sizeof(headers) / sizeof(char *);
```
Be sure to calculate the number of headers!

If needing a username and password, set before calling `simpleHttpRequest()`
```c
const char *userpass = "username:password";
simpleHttpSetPassword(&req, userpass, DIGEST);
```
If the server requires `DIGEST`, otherwise `NONE`

## Tests
I used [minunit](https://github.com/siu/minunit) for testing this library.

Build, using cmake with `cmake -DTARGET_GROUP:STRING=test ..` and the test executable will be in the build/test directory.

## Roadmap
Plans and lofty goals I have:
- Cookies
- Uploading/Downloading files
- Add in a logging system


## Authors and acknowledgment
I'd like to thank myself for contributing

## License
MIT License
