#include "../include/request.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VERBOSE
#define DEBUG(x, args...)                                                      \
  fprintf(stderr, " [%s(), %s:%u]\n" x, __FUNCTION__, __FILE__, __LINE__,      \
          ##args)
#else
#define DEBUG(x, args...)
#endif

static size_t writeCallback(void *, size_t, size_t, void *);

static size_t readCallback(char *, size_t, size_t, void *);

error simpleHttpInit(request *req) {
  DEBUG("Initializng library...\n");
  CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (res != CURLE_OK) {
    DEBUG("Something went wront initializing library: %s\n",
          curl_easy_strerror(res));
    return INIT_FAILED;
  }

  req->headers = NULL;
  req->numHeaders = 0;
  req->text = NULL;

  req->curl = curl_easy_init();

  if (req->curl == NULL) {
    DEBUG("Failed to initialize the library. Curl returned a null object\n");
    return INIT_FAILED;
  }
  DEBUG("Library successfully initialized and ready to use!\n");
  return NO_ERROR;
}

void simpleHttpClose(request *req, response *res) {
  DEBUG("Closing Library\n");
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

  DEBUG("Calling simpleHttpRequest\n");

  if (req->url == NULL) {
    DEBUG("No URL. Set the URL in the request object: request.url = {url}\n");
    return NO_URL;
  }

  DEBUG("Request Object:\n");
  DEBUG("\tURL: %s\n", req->url);
  if (req->text) {
    DEBUG("\tText: %s\n", req->text);
  }
  DEBUG("\tNumber of Custom Headers: %d\n", req->numHeaders);
  DEBUG("\tCustom Headers:\n");
  if (req->headers) {
    for (int i = 0; i < req->numHeaders; i++)
      DEBUG("\t\t%s\n", req->headers[i]);
  }

  res->body = NULL;

  setMethod(req, verb);

  // only if patch, post, put..
  if (verb != GET && verb != DELETE) {
    readBuffer buffer;
    buffer.readPtr = req->text;
    buffer.size = strlen(req->text);
    DEBUG("Setting a Read Buffer with text: %s\n\twith a size of: %ld\n",
          req->text, buffer.size);
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

  CURLcode curlRes = curl_easy_perform(req->curl);

  if (curlRes != CURLE_OK) {
    return REQUEST_FAILED;
  }

  curl_slist_free_all(headers);

  curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, &res->code);

  storeResponse(&chunk, res);
  return NO_ERROR;
}

void simpleHttpSetPassword(request *req, const char *userpass, digest dig) {

  if (userpass != NULL) {
    DEBUG("Setting username and password: %s\n", userpass);
    curl_easy_setopt(req->curl, CURLOPT_USERPWD, userpass);
    if (dig == DIGEST) {
      DEBUG("Setting HTTP Auth Digest on\n");
      curl_easy_setopt(req->curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);
    }
  } else {
    DEBUG("username and password is empty\n");
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
  DEBUG("Setting Request Options\n");

  // get the agent from lubcurl
  char agent[1024] = {0};
  snprintf(agent, sizeof agent, "libcurl/%s",
           curl_version_info(CURLVERSION_NOW)->version);
  agent[sizeof agent - 1] = 0;
  DEBUG("\tsetting agent: %s\n", agent);
  curl_easy_setopt(req->curl, CURLOPT_USERAGENT, agent);

  // set the URL option
  curl_easy_setopt(req->curl, CURLOPT_URL, req->url);
  DEBUG("\tsetting url: %s\n", req->url);

  // options that are standard for requests
  curl_easy_setopt(req->curl, CURLOPT_BUFFERSIZE, 102400L);
  DEBUG("\tsetting buffer size to 102400L\n");
  curl_easy_setopt(req->curl, CURLOPT_NOPROGRESS, 1L);
  DEBUG("\tsetting no progress on");
  curl_easy_setopt(req->curl, CURLOPT_MAXREDIRS, 50L);
  DEBUG("\tsetting max redirects to 50\n");
  curl_easy_setopt(req->curl, CURLOPT_HTTP_VERSION,
                   (long)CURL_HTTP_VERSION_2TLS);
  DEBUG("\tsetting HTTP version to 2 (HTTPS)\n");
  curl_easy_setopt(req->curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  DEBUG("\tsetting FTP skip (FTP SSL)\n");
  curl_easy_setopt(req->curl, CURLOPT_TCP_KEEPALIVE, 1L);
  DEBUG("\tsetting TCP keepalive\n");

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
  DEBUG("Setting Media Headers: ");

  // check if request.text contains data
  // if not, setting headers for return type
  if (req->text)
    curl_easy_setopt(req->curl, CURLOPT_POSTFIELDS, req->text);

  DEBUG("charset: utf-8\n");
  headers = curl_slist_append(headers, "charset: utf-8");

  switch (type) {
  case CSV:
    DEBUG("Accept: text/csv\n");
    DEBUG("Content-Type: text/csv\n");
    headers = curl_slist_append(headers, "Accept: text/csv");
    headers = curl_slist_append(headers, "Content-Type: text/csv");
    break;
  case JSON:
    DEBUG("Accept: application/json\n");
    DEBUG("Content-Type: application/json\n");
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    break;
  case XML:
    DEBUG("Accept: text/xml\n");
    DEBUG("Content-Type: text/xml\n");
    headers = curl_slist_append(headers, "Accept: text/xml");
    headers = curl_slist_append(headers, "Content-Type: text/xml");
    break;
  case HTML:
    DEBUG("Accept: text/html\n");
    DEBUG("Content-Type: text/html\n");
    headers = curl_slist_append(headers, "Accept: text/html");
    headers = curl_slist_append(headers, "Content-Type: text/html");
    break;
  case TEXT:
  default:
    DEBUG("Accept: text/plain\n");
    DEBUG("Content-Type: text/plain\n");
    headers = curl_slist_append(headers, "Accept: text/plain");
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    break;
  }

  curl_easy_setopt(req->curl, CURLOPT_HTTPHEADER, headers);
  DEBUG("Media headers set.\n");
}

void setMethod(request *req, method verb) {
  DEBUG("Setting HTTP Request method: ");
  if (verb == POST) {
    DEBUG("POST\n");
    curl_easy_setopt(req->curl, CURLOPT_POST, 1L);
  } else if (verb == PATCH) {
    DEBUG("PATCH\n");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PATCH");
  } else if (verb == PUT) {
    DEBUG("PUT\n");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "PUT");
  } else if (verb == DELETE) {
    DEBUG("DELETE\n");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  } else if (verb == GET) {
    DEBUG("GET\n");
    curl_easy_setopt(req->curl, CURLOPT_CUSTOMREQUEST, "GET");
  }
}

void setCustomHeaders(request *req, struct curl_slist *headers) {

  for (int i = 0; i < req->numHeaders; i++) {
    headers = curl_slist_append(headers, req->headers[i]);
  }
}
