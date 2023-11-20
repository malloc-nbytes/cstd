#define STDSTR_IMPL
#include "./cstd.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  StdStr s = stdstr_new();

  char *from = "foo bar baz";

  stdstr_append(&s, from);

  for (size_t i = 0; i < s.len; ++i) {
    printf("%c\n", s.data[i]);
  }

  return 0;
}
