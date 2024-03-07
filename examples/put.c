
#include "../include/request.h"

int main() {

  request req;
  response res;

  simpleHttpInit(&req);

  req.url = "https://httpbin.org/put";
  // data to update
  req.text = "{ \"name\": \"Jim Halpert\"}";

  error err = simpleHttpRequest(&req, &res, JSON, PUT);
  if (err != NO_ERROR)
    printf("error: %s\n", simpleHttpErrorString(err));

  printf("Response Code: %ld\n", res.code);
  printf("Response: %s\n", res.body);

  simpleHttpClose(&req, &res);
  return 0;
}
