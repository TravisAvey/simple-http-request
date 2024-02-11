#include "../include/request.h"

int main(int argc, char *argv[]) {

  request req;

  simpleHttpInit(&req);

  // req.text = "some text for post";
  req.url = "https://httpbin.org/post";
  req.text = "{ \"name\": \"Jim Halpert\"}";

  response res = simpleHttpRequest(&req, JSON, POST);

  printf("Response Code: %ld\n", res.code);
  printf("Response: %s\n", res.body);

  simpleHttpClose(&req, &res);
  return 0;
}
