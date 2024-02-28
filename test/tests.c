/*
 *  Tests...
 *
 *
 */

#include "../include/request.h"
#include "minunit/minunit.h"

static int foo;
static int bar;

void setup(void) {
  foo = 1;
  bar = 8;
}

void teardown(void) {
  // cleanup
}

MU_TEST(testing) { mu_check(foo == 1); }

MU_TEST_SUITE(testSuite) {
  MU_SUITE_CONFIGURE(&setup, &teardown);

  MU_RUN_TEST(testing);
}

int main() {
  request req;
  response res;
  // response res;
  simpleHttpInit(&req);

  simpleHttpClose(&req, &res);

  MU_RUN_SUITE(testSuite);
  MU_REPORT();

  return MU_EXIT_CODE;
}
