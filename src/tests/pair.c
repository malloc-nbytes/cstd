#define CUT_SUPPRESS_TESTS
#define CUT_ABORT_ON_FAIL
#define CUT_IMPL
#include "./cut.h"
#define STDPAIR_IMPL
#include "../cstd.h"

void
test_modifying_pair(void)
{
  int x = 1;
  float y = 2.f;
  int z = 99;
  StdPair p = stdpair_new(&x, &y);

  *(int *)stdpair_fst(&p) = z;

  cut_assert_eq(*(int *)stdpair_fst(&p), z);
}

void
test_basic_pair(void)
{
  int x = 1;
  float y = 2.f;
  StdPair p = stdpair_new(&x, &y);

  cut_assert_eq(*(int *)stdpair_fst(&p), x);
  cut_assert_eq(*(float *)stdpair_snd(&p), y);
}

int
main(void)
{
  CUT_BEGIN;
  test_basic_pair();
  test_modifying_pair();
  CUT_END;
  return 0;
}
