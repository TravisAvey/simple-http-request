#include "../include/request.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

error simpleHttpInit(request *req) {
  CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (res != CURLE_OK) {
    return INIT_FAILED;
  }
  req->curl = curl_easy_init();

  if (req->curl == NULL) {
    return INIT_FAILED;
  }
  return NO_ERROR;
}

void simpleHttpClose(request *req, response *res) {
  curl_easy_cleanup(req->curl);
  req->curl = NULL;

  req->url = NULL;
  req->headers = NULL;

  res->body = NULL;

  curl_global_cleanup();
}

void simpleHttpRequest(request *req, response *response, mediaType type,
                       method verb) {

  response->err = NO_ERROR;

  if (req->url == NULL) {
    response->err = NO_URL;
    return;
  }

  struct curl_slist *headers = NULL;
  setMediaHeaders(req, type, headers);

  setCustomHeaders(req, headers);

  setMethod(req, verb);

  // only if patch, post, put..
  if (verb != GET && verb != DELETE) {
    readBuffer buffer;
    buffer.readPtr = req->text;
    buffer.size = strlen(req->text);
    curl_easy_setopt(req->curl, CURLOPT_READFUNCTION, readCallback);
    curl_easy_setopt(req->curl, CURLOPT_READDATA, &buffer);
  }

  struct writeBuffer chunk = {0};
  setOpts(req, &chunk);

  CURLcode res = curl_easy_perform(req->curl);

  if (res != CURLE_OK) {
    response->err = REQUEST_FAILED;
    return;
  }
  curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, response->code);

  storeResponse(&chunk, response);

  curl_slist_free_all(headers);
  headers = NULL;
}

void simpleHttpSetPassword(request *req, char *userpass, digest dig) {

  if (userpass != NULL) {
    curl_easy_setopt(req->curl, CURLOPT_USERPWD, userpass);
    if (dig == DIGEST) {
      curl_easy_setopt(req->curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);
    }
  }
}

char *simpleHttpErrorString(error err) {
  char *errStr;

  if (err == NO_ERROR) {
    return "";
  } else if (err == INIT_FAILED) {
    errStr = "Failed to initialize library";
  } else if (err == NO_MEMORY) {
    errStr = "Not enough memory for allocation";
  } else if (err == REQUEST_FAILED) {
    errStr = "The request failed--could not perform the request";
  } else if (err == NO_URL) {
    errStr = "Missing URL. Need a location to perform a request";
  } else {
    errStr = "Something went wrong.";
  }

  return errStr;
}

size_t writeCallback(void *content, size_t size, size_t nmeb, void *userdata) {
  size_t actualSize = size * nmeb;
  struct writeBuffer *mem = (struct writeBuffer *)userdata;

  char *ptr = realloc(mem->data, mem->size + actualSize + 1);

  if (!ptr) {
    return 0;
  }
  mem->data = ptr;
  memcpy(&(mem->data[mem->size]), content, actualSize);
  mem->size += actualSize;
  mem->data[mem->size] = 0;

  return actualSize;
}

size_t readCallback(char *dest, size_t size, size_t nmemb, void *userp) {
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

void setOpts(request *req, writeBuffer *chunk) {

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
  curl_easy_setopt(req->curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(req->curl, CURLOPT_WRITEDATA, (void *)chunk);
}

void storeResponse(writeBuffer *chunk, response *res) {
  res->body = malloc(strlen(chunk->data) + 1);
  strcpy(res->body, chunk->data);
  free(chunk->data);
}

void setMediaHeaders(request *req, mediaType type, struct curl_slist *headers) {

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

void setMethod(request *req, method verb) {
  if (verb == POST)
    curl_easy_setopt(req->curl, CURLOPT_POST, 1L);
  else if (verb == PATCH)
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PATCH");
  else if (verb == PUT)
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PUT");
  else if (verb == DELETE)
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
}

void setCustomHeaders(request *req, struct curl_slist *headers) {

  for (int i = 0; i < req->numHeaders; i++) {
    headers = curl_slist_append(headers, req->headers[i]);
  }
}
