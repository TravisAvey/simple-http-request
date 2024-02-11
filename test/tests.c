/*
 *  Tests...
 *
 *
 */

#include "../include/request.h"

int main(int argc, char *argv[]) {
  request req;
  response res;
  // response res;
  simpleHttpInit(&req);

  simpleHttpClose(&req, &res);

  return 0;
}
