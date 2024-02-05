#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <curl/curl.h>

typedef struct request {
  CURL *curl;         // The CURL object
  char *url;          // The URL; where to send the request
  char *text;         // The text data to be sent
  char *file;         // absolute path to file to be sent (optional)
  long httpCode;      // The response code (200, 400, 500, etc)
  char *responseText; // The response data text
} request;

typedef enum { GET, POST, PUT, PATCH, DELETE } action;
typedef enum { CSV, JSON, HTML, TEXT, XML } mediaType;

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

// callback that curl will call during a get request
static size_t simpleHttpWriteCallback(void *, size_t, size_t, void *);

// callback that curl will call during a send request (POST)
static size_t simpleHttpReadCallback(char *, size_t, size_t, void *);

// get request
int simpleHttpGet(request *);

// post request
int simpleHttpPost(request *, mediaType);

// sets standard curl options
void simpleHttpSetOpts(request *, response *);

// saves the response text in the request object
void simpleHttpStoreResponse(response *, request *);

void simpleHttpSetMediaHeaders(request *, mediaType, struct curl_slist *);

#endif
