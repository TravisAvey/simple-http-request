#include "../include/request.h"

int main(int argc, char *argv[]) {

  request req;
  response res;

  simpleHttpInit(&req);

  // req.text = "some text for post";
  req.url = "https://httpbin.org/post";
  req.text = "{ \"name\": \"Jim Halpert\"}";

  simpleHttpRequest(&req, &res, JSON, POST);

  printf("Response Code: %ld\n", res.code);
  printf("Response: %s\n", res.body);

  simpleHttpClose(&req, &res);
  return 0;
}
