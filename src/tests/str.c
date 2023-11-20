#define CUT_IMPL
#include "./cut.h"
#define STDSTR_IMPL
#include "../cstd.h"
#include <stdio.h>

void
test_clearing_str(void)
{
  char s[] = "hello";
  StdStr str = stdstr_from(s);
  cut_assert_eq(str.len, 5);
  stdstr_clr(&str);
  cut_assert_eq(str.len, 0);
  stdstr_free(&str);
}

void
test_appending_a_str(void)
{
  char *s = "foo bar baz";
  StdStr str = stdstr_new();
  stdstr_append(&str, s);
  cut_assert_eq(str.len, 11);
  for (size_t i = 0; i < str.len; ++i) {
    cut_assert_eq(str.data[i], s[i]);
  }
  stdstr_free(&str);
}

void
test_creating_a_str_with_from(void)
{
  char s[] = "hello";
  StdStr str = stdstr_from(s);

  cut_assert_eq(str.len, 5);
  cut_assert_eq(str.cap, 8);

  for (size_t i = 0; i < str.len; ++i) {
    cut_assert_eq(str.data[i], s[i]);
  }

  stdstr_free(&str);
}

void
test_creating_a_str_and_push(void)
{
  StdStr str = stdstr_new();
  cut_assert_eq(str.len, 0);
  stdstr_push(&str, 'a');
  cut_assert_eq(str.len, 1);
  stdstr_push(&str, 'b');
  cut_assert_eq(str.len, 2);
  stdstr_push(&str, 'c');
  cut_assert_eq(str.len, 3);

  stdstr_free(&str);
}

int
main(void)
{
  CUT_BEGIN;
  test_creating_a_str_and_push();
  test_creating_a_str_with_from();
  test_appending_a_str();
  CUT_END;
  return 0;
}
