#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <curl/curl.h>

typedef struct request {
  CURL *curl; // The CURL object
  char *url;  // The URL; where to send the request
  char *text; // The text data to be sent
  long code;  // The response code (200, 400, 500, etc)
  char *body; // The response body text
} request;

// Media Types for posting data
typedef enum { CSV, JSON, HTML, TEXT, XML } mediaType;

// HTTP verbs for calling request
typedef enum { GET, POST, PUT, PATCH, DELETE } method;

// struct to hold the response data
typedef struct response {
  char *data;
  size_t size;
} response;

// struct to hold data to be sent
typedef struct readBuffer {
  const char *readPtr;
  size_t size;
} readBuffer;

// initializes the simple http request library
int simpleHttpInit();

// close and clean up the simple http request library
// should be called when all requests are done
void simpleHttpClose();

// sends a request based on method to supplied url
// in the request struct
int simpleHttpRequest(request *, mediaType, method);

// callback that curl will call during a get request
static size_t simpleHttpWriteCallback(void *, size_t, size_t, void *);

// callback that curl will call during a send request (POST)
static size_t simpleHttpReadCallback(char *, size_t, size_t, void *);

// sets the curl option HTTP request method
void simpleHttpSetMethod(request *, method);

// sets standard curl options
void simpleHttpSetOpts(request *, response *);

// saves the response text in the request object
void simpleHttpStoreResponse(response *, request *);

// sets the headers for a request
void simpleHttpSetMediaHeaders(request *, mediaType, struct curl_slist *);

#endif
