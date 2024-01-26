/*
 *   Show simple example of a GET request
 *
 *
 */

#include "../include/request.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  request req;
  int ret = init(&req);
  if (ret) {
    printf("error initializing...\n");
  } else {
    printf("initialzing libcurl was a success\n");
  }

  // ...
  printf("closing simple http request lib\n");
  close(&req);
  printf("http request lib closed\n");
  return 0;
}
