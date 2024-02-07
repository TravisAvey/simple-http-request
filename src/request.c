#include "../include/request.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * intializes the simple http request library
 *  This is required to be called before using
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
 * closes and cleans up the http simple request library
 *  Call this when done using the library
 */
void simpleHttpClose() { curl_global_cleanup(); }

/*
 *  Sends a HTTP Request.
 *
 *    Sending a Request:
 *      request.url is required to send any request
 *        - example: https://api.endpoint/
 *      request.text should be filled in with data being sent, such
 *      as JSON, text, etc to be sent (not required for GET/DELETE requests)
 *
 *    On successful response from the server:
 *      The request.code will be filled in with the server's response
 *      such as 200, 404, etc
 *      The request.body will be filled in the response body, such as
 *      using GET to retreive JSON
 *
 *  @req the request object that has the URL and optional upload data
 *  @type the Media Type expected (JSON, text, xml, etc)
 *  @verb the HTTP request Method (GET, POST, PUT, PATCH, DELETE)
 *
 *  @return 0 success, -1 fail
 *    Be sure to check the request.code and request.body for any issues
 *    from the server
 */
int simpleHttpRequest(request *req, mediaType type, method verb) {
  req->curl = curl_easy_init();

  if (req->curl == NULL) {
    printf("Error initializing library");
    return -1;
  }
  if (req->url == NULL) {
    printf("A URL is required to send an HTTP Request\n");
    return -1;
  }

  struct curl_slist *headers = NULL;
  simpleHttpSetMediaHeaders(req, type, headers);

  simpleHttpSetCustomHeaders(req, headers);

  simpleHttpSetMethod(req, verb);

  // only if patch, post, put..
  if (verb != GET && verb != DELETE) {
    readBuffer buffer;
    buffer.readPtr = req->text;
    buffer.size = strlen(req->text);
    curl_easy_setopt(req->curl, CURLOPT_READFUNCTION, simpleHttpReadCallback);
    curl_easy_setopt(req->curl, CURLOPT_READDATA, &buffer);
  }

  struct response chunk = {0};
  simpleHttpSetOpts(req, &chunk);

  CURLcode res = curl_easy_perform(req->curl);

  if (res != CURLE_OK) {
    printf("post request failed: %s\n", curl_easy_strerror(res));
    return -1;
  }
  curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, &req->code);

  simpleHttpStoreResponse(&chunk, req);

  curl_easy_cleanup(req->curl);
  req->curl = NULL;
  curl_slist_free_all(headers);
  headers = NULL;

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
  req->body = malloc(strlen(chunk->data) + 1);
  strcpy(req->body, chunk->data);
  free(chunk->data);
}

/*
 *  Sets the Media Headers for Content-Type
 *
 *  @req the request struct object
 *  @type the media type (JSON, XML, etc)
 *  @headers the slist object to add the headers to
 */
void simpleHttpSetMediaHeaders(request *req, mediaType type,
                               struct curl_slist *headers) {

  // check if request.text contains data
  // if not, setting headers for return type
  if (req->text)
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

/*
 *   Sets the HTTP Method 'Verb' for the request
 */
void simpleHttpSetMethod(request *req, method verb) {
  if (verb == POST)
    curl_easy_setopt(req->curl, CURLOPT_POST, 1L);
  else if (verb == PATCH)
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PATCH");
  else if (verb == PUT)
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PUT");
  else if (verb == DELETE)
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
}

/*
 *   Sets the custom headers to the request
 *
 *   @req the request object
 *   @headers the slist object to add the headers to
 */
void simpleHttpSetCustomHeaders(request *req, struct curl_slist *headers) {

  for (int i = 0; i < req->numHeaders; i++) {
    headers = curl_slist_append(headers, req->headers[i]);
  }
}
