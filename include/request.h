#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <curl/curl.h>

typedef struct request {
  CURL *curl;
  long code;
  char *url;
  char *text;
} request;

typedef enum { GET, POST, PUT, PATCH, DELETE } action;
typedef enum {
  JSON,
  HTML,
  TEXT,
  XML,
  ZIP,
  JPEG,
  PNG,
  SVG,
  OBJ,
  FORM_DATA
} mediaType;

typedef struct response {
  char *data;
  size_t size;
} response;

// initializes the simple http request library
void simpleHttpInit();

// close and clean up the simple http request library
// should be called when all requests are done
void simpleHttpClose(request *);

// callback that curl will call during a get request
static size_t simpleHttpWriteCallback(void *, size_t, size_t, void *);

// get request
int simpleHttpgGet(request *);

// sets standard curl options
void simpleHttpSetOpts(CURL *);

#endif
