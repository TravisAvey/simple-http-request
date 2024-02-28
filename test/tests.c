#include "../include/request.h"
#include "minunit/minunit.h"
#include "util/readfile.h"
#include <string.h>

const char *getUrl = "https://8r7q4.wiremockapi.cloud/get/1";
const char *postUrl = "https://8r7q4.wiremockapi.cloud/json";
const char *putUrl = "https://httpbin.org/put";
const char *patchUrl = "https://httpbin.org/patch";
const char *deleteUrl = "https://httpbin.org/delete";
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

MU_TEST_SUITE(GetTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(GetRequest);
}

MU_TEST(PostRequest) {
  req.url = postUrl;
  req.text = "{ \"msg\": \"hello, world\" }";

  const char *expectedRes = readFile("post.txt");

  error err = simpleHttpRequest(&req, &res, JSON, POST);

  mu_assert(err == NO_ERROR, "POST request returned an error");
  mu_assert(res.code == 201, "POST request did not return 201");
  mu_assert_string_eq(expectedRes, res.body);
}

MU_TEST_SUITE(PostTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(PostRequest);
}

int main() {

  MU_RUN_SUITE(InitSuite);
  MU_RUN_SUITE(GetTestSuite);
  MU_RUN_SUITE(PostTestSuite);

  MU_REPORT();

  return MU_EXIT_CODE;
}
