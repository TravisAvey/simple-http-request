/*
 *  Tests...
 *
 *
 */

#include "../include/request.h"

int main(int argc, char *argv[]) {
  simpleHttpInit();

  request req;
  simpleHttpClose(&req);

  return 0;
}
