#define CUT_SUPPRESS_TESTS
#define CUT_IMPL
#include "./cut.h"
#define STDSTACK_IMPL
#include "../cstd.h"

void
test_inserting_large_num_of_elems(void)
{
  StdStack s = stdstack_new(sizeof(int));
  size_t n = 1000;
  for (size_t i = 0; i < n; ++i) {
    stdstack_push(&s, &i);
  }

  size_t i = n-1;
  while (!stdstack_empty(&s)) {
    cut_assert_eq(*(int *)stdstack_peek(&s), (int)i);
    --i;
    stdstack_pop(&s);
  }

  stdstack_free(&s);
}

void
test_inserting_small_num_of_elems(void)
{
  StdStack s = stdstack_new(sizeof(int));
  size_t n = 10;
  for (size_t i = 0; i < n; ++i) {
    stdstack_push(&s, &i);
  }

  size_t i = 9;
  while (!stdstack_empty(&s)) {
    cut_assert_eq(*(int *)stdstack_peek(&s), (int)i);
    --i;
    stdstack_pop(&s);
  }

  stdstack_free(&s);
}

int
main(void)
{
  CUT_BEGIN;
  test_inserting_small_num_of_elems();
  test_inserting_large_num_of_elems();
  CUT_END;
  return 0;
}
