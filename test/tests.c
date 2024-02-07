/*
 *  Tests...
 *
 *
 */

#include "../include/request.h"

int main(int argc, char *argv[]) {
  request req;
  simpleHttpInit(&req);

  simpleHttpClose(&req);

  return 0;
}
