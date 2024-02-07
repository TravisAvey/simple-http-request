#include "../include/request.h"

int main(int argc, char *argv[]) {

  request req;

  simpleHttpInit(&req);

  // req.text = "some text for post";
  req.url = "https://httpbin.org/post";
  req.text = "{ \"name\": \"Jim Halpert\"}";

  simpleHttpRequest(&req, JSON, POST);

  printf("Response Code: %ld\n", req.code);
  printf("Response: %s\n", req.body);

  simpleHttpClose(&req);
  return 0;
}
