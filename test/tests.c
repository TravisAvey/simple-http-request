#include "../include/request.h"
#include "minunit/minunit.h"
#include "util/readfile.h"
#include <string.h>

const char *getUrl = "https://8r7q4.wiremockapi.cloud/get/1";
const char *postUrl = "https://8r7q4.wiremockapi.cloud/json";
const char *putUrl = "https://8r7q4.wiremockapi.cloud/json/2";
const char *patchUrl = "https://8r7q4.wiremockapi.cloud/address/8";
const char *deleteUrl = "https://8r7q4.wiremockapi.cloud/post/9";
// postman:password
// Authorization: Basic cG9zdG1hbjpwYXNzd29yZA==
const char *authUrl = "https://postman-echo.com/digest-auth";

request req;
response res;

void setup(void) { simpleHttpInit(&req); }
void teardown(void) { simpleHttpClose(&req, &res); }

MU_TEST(Init) {
  mu_assert(req.url == NULL, "initializing request.url is not null");
  mu_assert(req.curl != NULL, "initializing request.curl is null");
  mu_assert(req.text == NULL, "initializing request.test is not null");
  mu_assert(req.headers == NULL, "initializing request.headers is not null");
  mu_assert(req.numHeaders == 0, "initializing requst.numHeaders is no 0");
}

MU_TEST_SUITE(InitSuite) {

  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(Init);
}

MU_TEST(GetRequest) {

  req.url = getUrl;

  error err = simpleHttpRequest(&req, &res, TEXT, GET);
  mu_assert(err == NO_ERROR, "GET request returned an error");
  mu_assert(res.code == 200, "GET request did not return 200");
  mu_assert_string_eq(res.body, "here is some text");
}

MU_TEST(GetAuthRequest) {
  req.url = authUrl;

  const char *headers[] = {"Authorization: Basic cG9zdG1hbjpwYXNzd29yZA=="};
  req.headers = headers;
  req.numHeaders = sizeof(headers) / sizeof(char *);

  const char *userpass = "postman:password";
  simpleHttpSetPassword(&req, userpass, DIGEST);

  const char *expectedRes = readFile("auth.txt");
  if (!expectedRes) {
    mu_fail("Unable to read file: auth.txt to test results");
  }

  error err = simpleHttpRequest(&req, &res, TEXT, GET);

  mu_assert(err == NO_ERROR, "GET request returned an error");
  mu_assert(res.code == 200, "GET request did not return 200");
  mu_assert_string_eq(expectedRes, res.body);
  free((void *)expectedRes);
}

MU_TEST(GetNoUrl) {

  error err = simpleHttpRequest(&req, &res, TEXT, GET);
  mu_assert(err == NO_URL, "GET request did not return NO_URL");
}

MU_TEST_SUITE(GetTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(GetRequest);
  MU_RUN_TEST(GetAuthRequest);
  MU_RUN_TEST(GetNoUrl);
}

MU_TEST(PostRequest) {
  req.url = postUrl;
  req.text = "{ \"msg\": \"hello, world\" }";

  const char *expectedRes = readFile("post.txt");
  if (!expectedRes) {
    mu_fail("Unable to read file: post.txt to test results");
  }

  error err = simpleHttpRequest(&req, &res, JSON, POST);

  mu_assert(err == NO_ERROR, "POST request returned an error");
  mu_assert(res.code == 201, "POST request did not return 201");
  mu_assert_string_eq(expectedRes, res.body);
  free((void *)expectedRes);
}

MU_TEST_SUITE(PostTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(PostRequest);
}

MU_TEST(PutRequest) {
  req.url = putUrl;
  req.text = "{ \"name\": \"Jim Halpert\" }";

  const char *expectedRes = readFile("put.txt");
  if (!expectedRes) {
    mu_fail("Unable to read file: put.txt to test results");
  }

  error err = simpleHttpRequest(&req, &res, JSON, PUT);

  mu_assert(err == NO_ERROR, "PUT request returned an error");
  mu_assert(res.code == 200, "PUT request did not return 200");
  mu_assert_string_eq(expectedRes, res.body);
  free((void *)expectedRes);
}

MU_TEST_SUITE(PutTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(PutRequest);
}

MU_TEST(PatchRequest) {
  req.url = patchUrl;
  req.text = "{ \"address\": \"123 anywhere street\" }";

  const char *expectedRes = readFile("put.txt");
  if (!expectedRes) {
    mu_fail("Unable to read file: put.txt to test results");
  }

  error err = simpleHttpRequest(&req, &res, JSON, PATCH);

  mu_assert(err == NO_ERROR, "PATCH request returned an error");
  mu_assert(res.code == 200, "PATCH request did not return 200");
  mu_assert_string_eq(expectedRes, res.body);
  free((void *)expectedRes);
}

MU_TEST_SUITE(PatchTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(PatchRequest);
}

MU_TEST(DeleteRequest) {
  req.url = deleteUrl;

  const char *expectedRes = readFile("delete.txt");
  if (!expectedRes) {
    mu_fail("Unable to read file: delete.txt to test results");
  }

  error err = simpleHttpRequest(&req, &res, JSON, DELETE);

  mu_assert(err == NO_ERROR, "DELETE request returned an error");
  mu_assert(res.code == 200, "DELETE request did not return 200");
  mu_assert_string_eq(expectedRes, res.body);
  free((void *)expectedRes);
}

MU_TEST_SUITE(DeleteTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(DeleteRequest);
}

int main() {

  MU_RUN_SUITE(InitSuite);
  MU_RUN_SUITE(GetTestSuite);
  MU_RUN_SUITE(PostTestSuite);
  MU_RUN_SUITE(PutTestSuite);
  MU_RUN_SUITE(PatchTestSuite);
  MU_RUN_SUITE(DeleteTestSuite);

  MU_REPORT();

  return MU_EXIT_CODE;
}
