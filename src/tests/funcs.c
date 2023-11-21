#define CUT_ABORT_ON_FAIL
#define CUT_IMPL
#include "./cut.h"
#define STDFUNCS_IMPL
#include "../cstd.h"

int
is_positive(const void *x)
{
  return (*(int *)x) > -1;
}

int
is_negative(const void *x)
{
  return (*(int *)x) < 0;
}

int
greater_than_100(const void *x)
{
  return (*(int *)x) > 100;
}

void
test_none_of(void)
{
  int arr[] = {1,2,3,4,5};
  size_t n = sizeof(arr)/sizeof(*arr);
  cut_assert_true(stdnone_of(arr, sizeof(*arr), n, is_negative));
  cut_assert_false(stdnone_of(arr, sizeof(*arr), n, is_positive));
}

void
test_any_of(void)
{
  int arr[] = {1,2,-1,4,5};
  size_t n = sizeof(arr)/sizeof(*arr);
  cut_assert_true(stdany_of(arr, sizeof(*arr), n, is_negative));
  cut_assert_false(stdany_of(arr, sizeof(*arr), n, greater_than_100));
}

void
test_all_of(void)
{
  int arr[] = {1,2,3,4,5};
  size_t n = sizeof(arr)/sizeof(*arr);
  cut_assert_true(stdall_of(arr, sizeof(*arr), n, is_positive));
  cut_assert_false(stdall_of(arr, sizeof(*arr), n, is_negative));
}

void
test_iota_fill(int cur)
{
  int arr[5] = {0};
  size_t n = sizeof(arr)/sizeof(*arr);
  stdiota_fill(arr, n);

  for (size_t i = 0; i < n; ++i) {
    cut_assert_eq((size_t)arr[i], (size_t)cur+i+1);
  }
}

int
test_basic_iota(int cur)
{
  int a = stdiota();
  int b = stdiota();
  int c = stdiota();

  cut_assert_eq(a, cur+0);
  cut_assert_eq(b, cur+1);
  cut_assert_eq(c, cur+2);

  return c;
}

int
main(void)
{
  CUT_BEGIN;
  int last_iota = test_basic_iota(0);
  test_iota_fill(last_iota);
  test_all_of();
  test_any_of();
  test_none_of();
  CUT_END;
  return 0;
}
