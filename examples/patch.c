#include "../include/request.h"

int main(int argc, char *argv[]) {

  request req;

  simpleHttpInit();

  // req.text = "some text for post";
  req.url = "https://httpbin.org/patch";
  // data to update
  req.text = "{ \"name\": \"Jim Halpert\"}";

  simpleHttpRequest(&req, JSON, PATCH);

  printf("Response Code: %ld\n", req.code);
  printf("Response: %s\n", req.body);

  simpleHttpClose();
  return 0;
}
