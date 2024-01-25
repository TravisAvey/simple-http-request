#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <curl/curl.h>
/*
 *  request.h
 *
 *  Basic Usage for GET, POST, PUT, PATCH, DELETE
 *
 */

typedef struct {
  CURL *curl;
  long code;
  char *url;
  char *text;
} request;

// initialize the library
// returns 0 on success, -1 if fail
int init(request *);

// close the libCURL interface
void close(request *);

#endif
