#include "../include/request.h"
#include "minunit/minunit.h"
#include <string.h>

const char *getUrl =
    "https://gist.githubusercontent.com/TravisAvey/"
    "d567afe00fa20535b9762f353b446b9b/raw/"
    "8772c6779f935201fd934975eb636c8726e26112/simpleHttpRequest.txt";

const char *postUrl = "https://httpbin.org/response-headers?freeform=testing";
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
  mu_assert(strstr(res.body, "this is a test for a basic get request") != NULL,
            "GET request did not return expected body");
}

MU_TEST_SUITE(GetTestSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(GetRequest);
}

MU_TEST(PostRequest) {
  req.url = postUrl;
  const char *expectedRes = "{\"Content-Length\": \"92\", \"Content-Type\": "
                            "\"application/json\",\"freeform\": \"hello\" }";
  error err = simpleHttpRequest(&req, &res, JSON, POST);
  mu_assert(err == NO_ERROR, "POST request returned an error");
  mu_assert(res.code == 200, "POST request did not return 200");
  mu_assert(strstr(res.body, expectedRes) != NULL,
            "POST request did not return expected response body");
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
