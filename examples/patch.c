#include "../include/request.h"

int main() {

  request req;
  response res;

  simpleHttpInit(&req);

  // req.text = "some text for post";
  req.url = "https://httpbin.org/patch";
  // data to update
  req.text = "{ \"name\": \"Jim Halpert\"}";

  error err = simpleHttpRequest(&req, &res, JSON, PATCH);
  if (err != NO_ERROR)
    printf("error: %s\n", simpleHttpErrorString(err));

  printf("Response Code: %ld\n", res.code);
  printf("Response: %s\n", res.body);

  simpleHttpClose(&req, &res);
  return 0;
}
