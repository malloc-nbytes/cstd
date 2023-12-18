#define CUT_SUPPRESS_TESTS
#define CUT_IMPL
#include "./cut.h"
#define STDQUEUE_IMPL
#include "../cstd.h"

void
test_inserting_large_num_of_elems(void)
{
  StdQueue q = stdqueue_new(sizeof(int));
  size_t n = 1000;
  for (size_t i = 0; i < n; ++i) {
    stdqueue_enqueue(&q, &i);
  }

  size_t i = 0;
  while (!stdqueue_empty(&q)) {
    cut_assert_eq(*(int *)stdqueue_peek(&q), (int)i);
    ++i;
    stdqueue_dequeue(&q);
  }

  stdqueue_free(&q);
}

void
test_inserting_small_num_of_elems(void)
{
  StdQueue q = stdqueue_new(sizeof(int));
  size_t n = 10;
  for (size_t i = 0; i < n; ++i) {
    stdqueue_enqueue(&q, &i);
  }

  size_t i = 0;
  while (!stdqueue_empty(&q)) {
    cut_assert_eq(*(int *)stdqueue_peek(&q), (int)i);
    ++i;
    stdqueue_dequeue(&q);
  }

  stdqueue_free(&q);
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
