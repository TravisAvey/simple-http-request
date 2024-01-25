/*
 *
 * request.c
 *
 *
 */

#include "../include/request.h"
#include <stdlib.h>

// init the libCURL
// if failed returns -1
// success: 0
int init(request *req) {
  req->url = NULL;
  req->text = NULL;
  req->code = 0;

  req->url = calloc(1, 1);
  if (req->url == NULL) {
    return -1;
  }

  req->text = calloc(1, 1);
  if (req->text == NULL) {
    return -1;
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);
  req->curl = curl_easy_init();
  if (req->curl) {
    return 0;
  }
  return -1;
}

void close(request *req) { curl_easy_cleanup(req->curl); }
