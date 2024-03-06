# Simple HTTP Request

## Description
This is a simple HTTP request library that will perform simple HTTP requests:

```
GET, POST, PUT, PATCH, DELETE
```

### Motivation
I decided to make this library, OK it's nothing more than a wrapper around libCURL, anyways... I made this because I have other project ideas in mind that would make use of libCURL but wanted a simplier interface.

## Building

Clone this repo or download a release on the release page.

If cloning, build the library:
```
mkdir build
cd build
cmake -DTARGET_GROUP:STRING={release,test,examples} ..
make
```

You can also pass `-DVERBOSE=on` for debugging purposes (it will print a lot of information). This will build a library `libSimpleHttpRequest.a` located in build/src.

Build with your own code
`gcc main.c -o test -L. -lSimpleHttpRequest -lcurl` 

## Usage
First, you'll need to the libSimpleHttpRequest.a library and the request.h file.

Then include the header file:
`#include "request.h"`

Create both a request object and a response object:
```
request req;
response res;
```

If you peek inside the request.h file, you will see what each of these are:
```
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


Now that the request and response objects are instantiated, you can set the URL for where your request is going:
```
req.url = "https://example.com/v2/object";
```

## Authors and acknowledgment
I'd like to thank myself for contributing

## License
MIT License
