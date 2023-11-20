#define STDSTR_IMPL
#include "./cstd.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  StdStr str = stdstr_from("lhello worldl");
  stdstr_rmchar(&str, 'l');
  stdstr_print(&str);
  stdstr_free(&str);
  return 0;
}
