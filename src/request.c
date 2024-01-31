#include "../include/request.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * init - intializes the simple http request library
 *
 * retuns the struct request object
 */
void simpleHttpInit() { curl_global_init(CURL_GLOBAL_DEFAULT); }

/*
 * close - closes and cleans up the http simple request library
 */
void simpleHttpClose(request *req) { curl_global_cleanup(); }

/*
 * get - inits curl and performs a GET request
 *
 *  @req the request struct object
 */
int simpleHttpgGet(request *req) {

  req->curl = curl_easy_init();
  struct response chunk = {0};

  simpleHttpSetOpts(req->curl);
  curl_easy_setopt(req->curl, CURLOPT_URL, req->url);

  curl_easy_setopt(req->curl, CURLOPT_WRITEFUNCTION, simpleHttpWriteCallback);
  curl_easy_setopt(req->curl, CURLOPT_WRITEDATA, (void *)&chunk);

  CURLcode res = curl_easy_perform(req->curl);

  if (res != CURLE_OK) {
    printf("get request failed: %s\n", curl_easy_strerror(res));
    return -1;
  }
  curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, &req->code);
  curl_easy_cleanup(req->curl);

  req->text = malloc(strlen(chunk.data) + 1);
  strcpy(req->text, chunk.data);
  free(chunk.data);

  return 0;
}

size_t simpleHttpWriteCallback(void *content, size_t size, size_t nmeb,
                               void *userdata) {
  size_t actualSize = size * nmeb;
  struct response *mem = (struct response *)userdata;

  char *ptr = realloc(mem->data, mem->size + actualSize + 1);

  if (!ptr) {
    printf("Not enough memory to allocate for the callback\n");
    return 0;
  }
  mem->data = ptr;
  memcpy(&(mem->data[mem->size]), content, actualSize);
  mem->size += actualSize;
  mem->data[mem->size] = 0;

  return actualSize;
}

void simpleHttpSetOpts(CURL *curl) {

  curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  // TODO: get version of curl here..
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/8.5.0");
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
}
