#define CUT_ABORT_ON_FAIL
#define CUT_IMPL
#include "./cut.h"
#define STDFUNCS_IMPL
#include "../cstd.h"

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
  CUT_END;
  return 0;
}
