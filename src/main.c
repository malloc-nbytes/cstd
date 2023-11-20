#define STDSTR_IMPL
#include "./cstd.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  StdStr s = stdstr_new();

  char *from = "foo bar baz";

  stdstr_append(&s, from);
  
  stdstr_print(&s);

  return 0;
}
