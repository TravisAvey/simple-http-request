#include "../include/request.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VERBOSE
#define DEBUG(x, args...)                                                      \
  fprintf(stderr, "  %s:%u\n" x, __FILE__, __LINE__, ##args)
#else
#define DEBUG(x, args...)
#endif

static size_t writeCallback(void *, size_t, size_t, void *);

static size_t readCallback(char *, size_t, size_t, void *);

error simpleHttpInit(request *req) {
  DEBUG("Initializng library...");
  CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (res != CURLE_OK) {
    DEBUG("Something went wront initializing library: %s",
          curl_easy_strerror(res));
    return INIT_FAILED;
  }

  req->headers = NULL;
  req->numHeaders = 0;
  req->text = NULL;

  req->curl = curl_easy_init();

  if (req->curl == NULL) {
    DEBUG("Failed to initialize the library. Curl returned a null object");
    return INIT_FAILED;
  }
  DEBUG("Library successfully initialized and ready to use!\n");
  return NO_ERROR;
}

void simpleHttpClose(request *req, response *res) {
  DEBUG("Closing Library");
  curl_easy_cleanup(req->curl);
  req->curl = NULL;

  req->url = NULL;
  req->headers = NULL;
  req->text = NULL;
  res->body = NULL;

  curl_global_cleanup();
  DEBUG("Simple HTTP Request library closed\n");
}

error simpleHttpRequest(request *req, response *res, mediaType type,
                        method verb) {

  DEBUG("Calling simpleHttpRequest");

  if (req->url == NULL) {
    DEBUG("No URL. Set the URL in the request object: request.url = {url}");
    return NO_URL;
  }

  DEBUG("Request Object:");
  DEBUG("URL: %s", req->url);
  if (req->text) {
    DEBUG("Text: %s", req->text);
  }
  DEBUG("Number of Custom Headers: %d", req->numHeaders);
  DEBUG("Custom Headers:");
  if (req->headers) {
    for (int i = 0; i < req->numHeaders; i++)
      DEBUG("\t\t%s", req->headers[i]);
  }

  res->body = NULL;

  setMethod(req, verb);

  // only if patch, post, put..
  if (verb != GET && verb != DELETE) {
    readBuffer buffer;
    buffer.readPtr = req->text;
    buffer.size = strlen(req->text);
    DEBUG("Setting a Read Buffer with text: %s\nwith a size of: %ld", req->text,
          buffer.size);
    curl_easy_setopt(req->curl, CURLOPT_READFUNCTION, readCallback);
    curl_easy_setopt(req->curl, CURLOPT_READDATA, &buffer);
  }

  struct writeBuffer chunk = {0};
  setOpts(req, &chunk);

  struct curl_slist *headers = NULL;
  if (req->headers) {
    setCustomHeaders(req, headers);
  }

  setMediaHeaders(req, type, headers);

  DEBUG("Performing request...");
  CURLcode curlRes = curl_easy_perform(req->curl);

  if (curlRes != CURLE_OK) {
    DEBUG("Request failed: %s", curl_easy_strerror(curlRes));
    return REQUEST_FAILED;
  } else {
    DEBUG("Request succeeded: %s", curl_easy_strerror(curlRes));
  }

  DEBUG("Freeing all headers");
  curl_slist_free_all(headers);

  curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, &res->code);
  DEBUG("Server responded with code: %ld", res->code);

  storeResponse(&chunk, res);
  DEBUG("Successfully completed the request");
  return NO_ERROR;
}

void simpleHttpSetPassword(request *req, const char *userpass, digest dig) {

  if (userpass != NULL) {
    DEBUG("Setting username and password: %s", userpass);
    curl_easy_setopt(req->curl, CURLOPT_USERPWD, userpass);
    if (dig == DIGEST) {
      DEBUG("Setting HTTP Auth Digest on");
      curl_easy_setopt(req->curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);
    }
  } else {
    DEBUG("username and password is empty");
  }
}

const char *simpleHttpErrorString(error err) {

  if (err == NO_ERROR) {
    return "";
  } else if (err == INIT_FAILED) {
    return "Failed to initialize library";
  } else if (err == NO_MEMORY) {
    return "Not enough memory for allocation";
  } else if (err == REQUEST_FAILED) {
    return "The request failed--could not perform the request";
  } else if (err == NO_URL) {
    return "Missing URL. Need a location to perform a request";
  } else {
    return "Something went wrong.";
  }

  return "";
}

static size_t writeCallback(void *content, size_t size, size_t nmeb,
                            void *userdata) {
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

static size_t readCallback(char *dest, size_t size, size_t nmemb, void *userp) {
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
  DEBUG("Setting Request Options");

  // get the agent from lubcurl
  char agent[1024] = {0};
  snprintf(agent, sizeof agent, "libcurl/%s",
           curl_version_info(CURLVERSION_NOW)->version);
  agent[sizeof agent - 1] = 0;
  DEBUG("\tsetting agent: %s", agent);
  curl_easy_setopt(req->curl, CURLOPT_USERAGENT, agent);

  // set the URL option
  curl_easy_setopt(req->curl, CURLOPT_URL, req->url);
  DEBUG("\tsetting url: %s", req->url);

  // options that are standard for requests
  curl_easy_setopt(req->curl, CURLOPT_BUFFERSIZE, 102400L);
  DEBUG("\tsetting buffer size to 102400L");
  curl_easy_setopt(req->curl, CURLOPT_NOPROGRESS, 1L);
  DEBUG("\tsetting no progress on");
  curl_easy_setopt(req->curl, CURLOPT_MAXREDIRS, 50L);
  DEBUG("\tsetting max redirects to 50");
  curl_easy_setopt(req->curl, CURLOPT_HTTP_VERSION,
                   (long)CURL_HTTP_VERSION_2TLS);
  DEBUG("\tsetting HTTP version to 2 (HTTPS)");
  curl_easy_setopt(req->curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  DEBUG("\tsetting FTP skip (FTP SSL)");
  curl_easy_setopt(req->curl, CURLOPT_TCP_KEEPALIVE, 1L);
  DEBUG("\tsetting TCP keepalive");

  // curl options to save the response text
  curl_easy_setopt(req->curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(req->curl, CURLOPT_WRITEDATA, (void *)chunk);
}

void storeResponse(writeBuffer *chunk, response *res) {
  DEBUG("Storing response body from server into response struct");
  res->body = malloc(strlen(chunk->data) + 1);
  strcpy(res->body, chunk->data);
  free(chunk->data);
  DEBUG("Response body stored");
}

void setMediaHeaders(request *req, mediaType type, struct curl_slist *headers) {
  DEBUG("Setting Media Headers: ");

  // check if request.text contains data
  // if not, setting headers for return type
  if (req->text)
    curl_easy_setopt(req->curl, CURLOPT_POSTFIELDS, req->text);

  DEBUG("charset: utf-8");
  headers = curl_slist_append(headers, "charset: utf-8");

  switch (type) {
  case CSV:
    DEBUG("Accept: text/csv");
    DEBUG("Content-Type: text/csv");
    headers = curl_slist_append(headers, "Accept: text/csv");
    headers = curl_slist_append(headers, "Content-Type: text/csv");
    break;
  case JSON:
    DEBUG("Accept: application/json");
    DEBUG("Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    break;
  case XML:
    DEBUG("Accept: text/xml");
    DEBUG("Content-Type: text/xml");
    headers = curl_slist_append(headers, "Accept: text/xml");
    headers = curl_slist_append(headers, "Content-Type: text/xml");
    break;
  case HTML:
    DEBUG("Accept: text/html");
    DEBUG("Content-Type: text/html");
    headers = curl_slist_append(headers, "Accept: text/html");
    headers = curl_slist_append(headers, "Content-Type: text/html");
    break;
  case TEXT:
  default:
    DEBUG("Accept: text/plain");
    DEBUG("Content-Type: text/plain");
    headers = curl_slist_append(headers, "Accept: text/plain");
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    break;
  }

  curl_easy_setopt(req->curl, CURLOPT_HTTPHEADER, headers);
  DEBUG("Media headers set.");
}

void setMethod(request *req, method verb) {
  DEBUG("Setting HTTP Request method: ");
  if (verb == POST) {
    DEBUG("POST");
    curl_easy_setopt(req->curl, CURLOPT_POST, 1L);
  } else if (verb == PATCH) {
    DEBUG("PATCH");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PATCH");
  } else if (verb == PUT) {
    DEBUG("PUT");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PUT");
  } else if (verb == DELETE) {
    DEBUG("DELETE");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  } else if (verb == GET) {
    DEBUG("GET");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "GET");
  }
}

void setCustomHeaders(request *req, struct curl_slist *headers) {

  DEBUG("Settin custom headers:");
  for (int i = 0; i < req->numHeaders; i++) {
    DEBUG("%s", req->headers[i]);
    headers = curl_slist_append(headers, req->headers[i]);
  }
}
