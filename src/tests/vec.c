#define CUT_SUPPRESS_TESTS
#define CUT_ABORT_ON_FAIL
#define CUT_IMPL
#include "./cut.h"
#define STDVEC_IMPL
#include "../cstd.h"
#include <stdio.h>

void
test_push_small_number_of_elems(void)
{
  StdVec vec = stdvec_create(sizeof(int));
  for (int i = 0; i < 10; ++i) {
    stdvec_push(&vec, &i);
  }

  for (int i = 0; i < 10; ++i) {
    cut_assert_eq(*(int *)stdvec_at(&vec, i), i);
  }

  stdvec_free(&vec);
}

int
main(void)
{
  CUT_BEGIN;
  test_push_small_number_of_elems();
  CUT_END;
  return 0;
}
