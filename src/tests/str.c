#define CUT_ABORT_ON_FAIL
#define CUT_SUPPRESS_TESTS
#define CUT_IMPL
#include "./cut.h"
#define STDSTR_IMPL
#include "../cstd.h"
#include <stdio.h>

#define FILEPATH1 "./sample-files/basic-words-multiline.txt"

void
test_str_to_cstr(void)
{
  char *s = "hello world";
  StdStr str = stdstr_from(s);
  char *cstr = stdstr_to_cstr(&str);
  cut_assert_eqstr(cstr, s);
  stdstr_free(&str);
  free(cstr);
}

void
test_removing_all_chars_matching_value(void)
{
  char *s = "lhello worldl";
  StdStr str = stdstr_from(s);
  cut_assert_eq(str.len, 13);

  stdstr_rmchar(&str, 'l');
  cut_assert_eq(str.len, 8);

  cut_assert_eq(str.data[0], 'h');
  cut_assert_eq(str.data[1], 'e');
  cut_assert_eq(str.data[2], 'o');
  cut_assert_eq(str.data[3], ' ');
  cut_assert_eq(str.data[4], 'w');
  cut_assert_eq(str.data[5], 'o');
  cut_assert_eq(str.data[6], 'r');
  cut_assert_eq(str.data[7], 'd');

  stdstr_free(&str);
}

void
test_reading_from_file(void)
{
  StdStr str = stdstr_from_file(FILEPATH1);

  char *buf = 0;
  long len;

  FILE *fp = fopen(FILEPATH1, "r");

  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  buf = __STD_S_MALLOC(len);
  fread(buf, 1, len, fp);

  for (long i = 0; i < len; ++i) {
    cut_assert_eq(str.data[i], buf[i]);
  }

  free(buf);
  fclose(fp);
  stdstr_free(&str);
}

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
  test_reading_from_file();
  test_removing_all_chars_matching_value();
  test_str_to_cstr();
  CUT_END;
  return 0;
}
