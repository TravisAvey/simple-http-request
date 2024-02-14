#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <curl/curl.h>

// Media Types for posting data
typedef enum { CSV, JSON, HTML, TEXT, XML } mediaType;

// HTTP verbs for calling request
typedef enum { GET, POST, PUT, PATCH, DELETE } method;

// Digest for auth on or off
typedef enum { DIGEST, NONE } digest;

// Error types, 0 is no error
typedef enum {
  NO_ERROR = 0,      // success
  INIT_FAILED = 1,   // The initialization of the library failed
  NO_URL = 2,        // Missing URL for the request
  NO_MEMORY = 3,     // Couldn't initialize due to not enough memory
  REQUEST_FAILED = 4 // The request failed -- not a server error
} error;

typedef struct request {
  CURL *curl;           // The CURL object
  const char *url;      // The URL; where to send the request
  const char *text;     // The text data to be sent
  const char **headers; // An array of custom headers
  int numHeaders;       // The number of custom headers
} request;

typedef struct response {
  long code;  // The response code (200, 400, 500, etc)
  char *body; // The response body text
} response;

// struct to hold the response data
typedef struct writeBuffer {
  char *data;
  size_t size;
} writeBuffer;

// struct to hold data to be sent
typedef struct readBuffer {
  const char *readPtr;
  size_t size;
} readBuffer;

/*
 * intializes the simple http request library
 *  This is required to be called before using
 *
 * @req the request struct object
 *
 * @return NO_ERROR on success
 */
error simpleHttpInit(request *);

/*
 * closes and cleans up the http simple request library
 *  Call this when done using the library
 *
 *  @req the request struct object
 */
void simpleHttpClose(request *, response *);

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
 *  @return NO_ERROR if successful
 *    Be sure to check the request.code and request.body for any issues
 *    from the server
 */
error simpleHttpRequest(request *, response *, mediaType, method);

/*
 *  Sets the username password for a request
 *
 *  @req the request struct object
 *  @userpass the username and password (set as "username:password")
 *  @dig if digest auth is needed (set DIGEST), else set as NONE
 *
 */
void simpleHttpSetPassword(request *, const char *, digest);

/*
 *  Returs the error code as a helpful string
 *
 *  @err the error code
 *
 *  @return the error as a string
 */
const char *simpleHttpErrorString(error);

/*
 *   Sets the custom headers to the request
 *
 *   @req the request object
 *   @headers the slist object to add the headers to
 */
void setCustomHeaders(request *, struct curl_slist *);

/*
 *   Sets the HTTP Method 'Verb' for the request
 */
void setMethod(request *, method);

/*
 *  Sets the standard CURL options
 *
 *  @req the requst object
 *
 *  @chunk the buffer to store the response
 */
void setOpts(request *, writeBuffer *);

/*
 *  saves the response text into the request object
 *
 *  @chunk the write callback buffer
 *  @req the request object
 */
void storeResponse(writeBuffer *, response *);

/*
 *  Sets the Media Headers for Content-Type
 *
 *  @req the request struct object
 *  @type the media type (JSON, XML, etc)
 *  @headers the slist object to add the headers to
 */
void setMediaHeaders(request *, mediaType, struct curl_slist *);

void verbosePrint(const char *);

#endif
