#include "../include/request.h"

int main(int argc, char *argv[]) {

  request req;

  simpleHttpInit();

  // req.text = "some text for post";
  req.url = "https://httpbin.org/post";
  req.text = "{ \"name\": \"james\"}";

  simpleHttpPost(&req, JSON);

  printf("Response Code: %ld\n", req.httpCode);
  printf("Response: %s\n", req.responseText);

  simpleHttpClose();
  return 0;
}
