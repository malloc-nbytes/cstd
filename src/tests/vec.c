#define CUT_SUPPRESS_TESTS
#define CUT_ABORT_ON_FAIL
#define CUT_IMPL
#include "./cut.h"
#define STDVEC_IMPL
#include "../cstd.h"
#include <stdio.h>

void
map_func1(void *x)
{
  *(int *)x *= 2;
}

int
compar_func1(const void *x, const void *y)
{
  return *(int *)x > *(int *)y;
}

void
test_reverse(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  int arr[] = {5,4,3,2,1};
  size_t n = sizeof(arr)/sizeof(*arr);
  for (size_t i = 0; i < n; ++i) {
    stdvec_push(&vec, &arr[i]);
  }

  stdvec_rev(&vec);

  for (size_t i = 0; i < n; ++i) {
    cut_assert_eq(*(int *)stdvec_at(&vec, i), (int)i+1);
  }

  stdvec_free(&vec);
}

void
test_qsort(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  int arr[] = {5,4,3,2,1};
  size_t n = sizeof(arr)/sizeof(*arr);
  for (size_t i = 0; i < n; ++i) {
    stdvec_push(&vec, &arr[i]);
  }

  stdvec_qsort(&vec, compar_func1);

  for (size_t i = 0; i < n; ++i) {
    cut_assert_eq(*(int *)stdvec_at(&vec, i), (int)i+1);
  }

  stdvec_free(&vec);
}

void
test_map(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  int arr[] = {1,2,3,4,5};
  size_t n = sizeof(arr)/sizeof(*arr);
  for (size_t i = 0; i < n; ++i) {
    stdvec_push(&vec, &arr[i]);
  }

  StdVec mapped = stdvec_map(&vec, map_func1);

  for (size_t i = 0; i < n; ++i) {
    cut_assert_eq(*(int *)stdvec_at(&mapped, i), arr[i]*2);
  }

  stdvec_free(&mapped);
}

void
test_clear(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  size_t n = 10;
  for (int i = 0; i < (int)n; ++i) {
    stdvec_push(&vec, &i);
  }

  cut_assert_eq(vec.len, n);

  stdvec_clr(&vec);
  cut_assert_eq(vec.len, 0);

  stdvec_free(&vec);
}

void
test_contains(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  size_t n = 10;
  for (int i = 0; i < (int)n; ++i) {
    stdvec_push(&vec, &i);
  }

  for (int i = 0; i < (int)n; ++i) {
    cut_assert_true(stdvec_contains(&vec, &i));
  }

  cut_assert_false(stdvec_contains(&vec, STDCL(int, 99)));

  stdvec_free(&vec);
}

void
test_remove(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  stdvec_push(&vec, STDCL(int, 1));
  stdvec_push(&vec, STDCL(int, 2));
  stdvec_push(&vec, STDCL(int, 1));
  stdvec_push(&vec, STDCL(int, 1));
  stdvec_push(&vec, STDCL(int, 3));
  stdvec_push(&vec, STDCL(int, 1));
  stdvec_push(&vec, STDCL(int, 1));
  cut_assert_eq(vec.len, 7);

  stdvec_rm(&vec, STDCL(int, 1));
  cut_assert_eq(vec.len, 2);
  cut_assert_false(stdvec_contains(&vec, STDCL(int, 1)));

  stdvec_free(&vec);
}

void
test_remove_at(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  size_t n = 10;
  for (int i = 0; i < (int)n; ++i) {
    stdvec_push(&vec, &i);
  }

  cut_assert_eq(vec.len, n);
  stdvec_rm_at(&vec, 2);
  cut_assert_eq(vec.len, n-1);
  cut_assert_false(stdvec_contains(&vec, STDCL(int, 2)));

  stdvec_rm_at(&vec, vec.len-1);
  cut_assert_eq(vec.len, n-2);
  cut_assert_false(stdvec_contains(&vec, STDCL(int, 9)));

  stdvec_free(&vec);
}

void
test_create_with_capacity(void)
{
  int n = 10;
  StdVec vec = stdvec_wcap(sizeof(int), n);
  cut_assert_eq(vec.cap, (size_t)n);

  for (int i = 0; i < n; ++i) {
    stdvec_push(&vec, &i);
  }
  cut_assert_eq(vec.len, (size_t)n);

  stdvec_free(&vec);
}

void
test_push_large_number_of_elems(void)
{
  StdVec vec = stdvec_new(sizeof(int));
  int n = 1000;
  for (int i = 0; i < n; ++i) {
    stdvec_push(&vec, &i);
  }

  for (int i = 0; i < n; ++i) {
    cut_assert_eq(*(int *)stdvec_at(&vec, i), i);
  }

  stdvec_free(&vec);
}

void
test_push_small_number_of_elems(void)
{
  StdVec vec = stdvec_new(sizeof(int));
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
  test_push_large_number_of_elems();
  test_create_with_capacity();
  test_remove_at();
  test_remove();
  test_contains();
  test_clear();
  test_map();
  test_qsort();
  test_reverse();
  CUT_END;
  return 0;
}
