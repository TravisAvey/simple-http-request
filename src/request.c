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
 * returns 0 on success; -1 on fail
 */
int simpleHttpInit() {
  CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (res != CURLE_OK) {
    printf("Error initializing library: %s\n", curl_easy_strerror(res));
    return -1;
  }
  return 0;
}

/*
 * close - closes and cleans up the http simple request library
 */
void simpleHttpClose() { curl_global_cleanup(); }

/*
 * get - inits curl and performs a GET request
 *
 *  @req the request struct object
 */
int simpleHttpGet(request *req) {

  req->curl = curl_easy_init();
  if (req->curl == NULL) {
    printf("Error initializing library\n");
    return -1;
  }
  struct response chunk = {0};

  simpleHttpSetOpts(req, &chunk);

  CURLcode res = curl_easy_perform(req->curl);

  if (res != CURLE_OK) {
    printf("get request failed: %s\n", curl_easy_strerror(res));
    return -1;
  }
  curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, &req->httpCode);
  curl_easy_cleanup(req->curl);

  simpleHttpStoreResponse(&chunk, req);

  return 0;
}

/*
 *  The callback for receiving data from a request
 */
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

/*
 *  Sends a POST request
 *
 *  @req the request object.
 *    set the URL request.url
 *    the response will be saved
 *    in request.responseData
 *  @type the Media Type for the POST headers
 *
 *  @return 0 success and -1 fail
 */
int simpleHttpPost(request *req, mediaType type) {
  req->curl = curl_easy_init();

  if (req->curl == NULL) {
    printf("Error initializing library");
    return -1;
  }

  readBuffer buffer;
  buffer.readPtr = req->text;
  buffer.size = strlen(req->text);

  struct response chunk = {0};

  struct curl_slist *headers = NULL;
  simpleHttpSetMediaHeaders(req, type, headers);

  curl_easy_setopt(req->curl, CURLOPT_POST, 1L);
  curl_easy_setopt(req->curl, CURLOPT_READFUNCTION, simpleHttpReadCallback);
  curl_easy_setopt(req->curl, CURLOPT_READDATA, &buffer);

  simpleHttpSetOpts(req, &chunk);

  CURLcode res = curl_easy_perform(req->curl);

  if (res != CURLE_OK) {
    printf("post request failed: %s\n", curl_easy_strerror(res));
    return -1;
  }
  curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, &req->httpCode);

  simpleHttpStoreResponse(&chunk, req);

  curl_easy_cleanup(req->curl);
  req->curl = NULL;
  curl_slist_free_all(headers);
  headers = NULL;
  return 0;
}

/*
 *  The callback used for sending data on a request
 *
 */
size_t simpleHttpReadCallback(char *dest, size_t size, size_t nmemb,
                              void *userp) {
  readBuffer *buffer = (struct readBuffer *)userp;
  size_t bufSize = size * nmemb;

  if (buffer->size) {
    size_t copyAmount = buffer->size;
    if (copyAmount > bufSize) {
      copyAmount = bufSize;
    }
    memcpy(dest, buffer->readPtr, copyAmount);

    buffer->readPtr += copyAmount;
    buffer->size -= copyAmount;
    return copyAmount;
  }
  return 0;
}

/*
 *  Sets the standard CURL options
 *
 *  @req the requst object
 *
 *  @chunk the buffer to store the response
 */
void simpleHttpSetOpts(request *req, response *chunk) {

  // get the agent from lubcurl
  char agent[1024] = {0};
  snprintf(agent, sizeof agent, "libcurl/%s",
           curl_version_info(CURLVERSION_NOW)->version);
  agent[sizeof agent - 1] = 0;
  curl_easy_setopt(req->curl, CURLOPT_USERAGENT, agent);

  // set the URL option
  curl_easy_setopt(req->curl, CURLOPT_URL, req->url);

  // options that are standard for requests
  curl_easy_setopt(req->curl, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(req->curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(req->curl, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(req->curl, CURLOPT_HTTP_VERSION,
                   (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(req->curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(req->curl, CURLOPT_TCP_KEEPALIVE, 1L);

  // curl options to save the response text
  curl_easy_setopt(req->curl, CURLOPT_WRITEFUNCTION, simpleHttpWriteCallback);
  curl_easy_setopt(req->curl, CURLOPT_WRITEDATA, (void *)chunk);
}

/*
 *  saves the response text into the request object
 *
 *  @chunk the write callback buffer
 *  @req the request object
 */
void simpleHttpStoreResponse(response *chunk, request *req) {
  req->responseText = malloc(strlen(chunk->data) + 1);
  strcpy(req->responseText, chunk->data);
  free(chunk->data);
}

void simpleHttpSetMediaHeaders(request *req, mediaType type,
                               struct curl_slist *headers) {

  curl_easy_setopt(req->curl, CURLOPT_POSTFIELDS, req->text);
  headers = curl_slist_append(headers, "charset: utf-8");

  switch (type) {
  case CSV:
    headers = curl_slist_append(headers, "Accept: text/csv");
    headers = curl_slist_append(headers, "Content-Type: text/csv");
    break;
  case JSON:
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    break;
  case XML:
    headers = curl_slist_append(headers, "Accept: text/xml");
    headers = curl_slist_append(headers, "Content-Type: text/xml");
    break;
  case HTML:
    headers = curl_slist_append(headers, "Accept: text/html");
    headers = curl_slist_append(headers, "Content-Type: text/html");
    break;
  case TEXT:
  default:
    headers = curl_slist_append(headers, "Accept: text/plain");
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    break;
  }

  curl_easy_setopt(req->curl, CURLOPT_HTTPHEADER, headers);
}
