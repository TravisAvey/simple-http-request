#include "../include/request.h"
#include <curl/curl.h>
#include <stdlib.h>

/*
 * init - intializes the simple http request library
 *
 * @req the request struct
 *
 * returns 0 on success, -1 of fail
 */
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
  if (req->curl == NULL) {
    free(req->url);
    free(req->text);
    curl_global_cleanup();
    return -1;
  }
  return 0;
}

/*
 * close - closes and cleans up the http simple request library
 *
 * @req the request struct
 */
void close(request *req) {
  free(req->url);
  free(req->text);

  curl_easy_cleanup(req->curl);

  curl_global_cleanup();
}
