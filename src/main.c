#define STDSTR_IMPL
#include "./cstd.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  StdStr s = stdstr_from_file("./input.txt");
  stdstr_print(&s);
  stdstr_free(&s);

  return 0;
}
