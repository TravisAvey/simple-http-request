#include "../include/request.h"

int main(int argc, char *argv[]) {

  request req;

  simpleHttpInit();

  // req.text = "some text for post";
  req.url = "https://httpbin.org/post";
  req.text = "{ \"name\": \"Jim Halpert\"}";

  simpleHttpPost(&req, JSON);

  printf("Response Code: %ld\n", req.code);
  printf("Response: %s\n", req.body);

  simpleHttpClose();
  return 0;
}
