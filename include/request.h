#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <curl/curl.h>

typedef struct {
  CURL *curl;
  long code;
  char *url;
  char *text;
} request;

// initializes the simple http request library
// returns 0 on success, -1 of fail
int init(request *);

// close and clean up the simple http request library
// should be called when all requests are done
void close(request *);

#endif
