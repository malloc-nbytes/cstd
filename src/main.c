#define STDVEC_IMPL
#include "./cstd.h"
#include <stdio.h>
#include <limits.h>

void mapfunc(void *x) {
  char *s = (char*)x;
  s[0] = 'a';
}

int
main(void)
{
  StdVec vec = stdvec_create(sizeof(char*));

  char *s1 = "hello";
  char *s2 = "world";

  stdvec_push(&vec, s1);
  stdvec_push(&vec, s2);

  StdVec mapped = stdvec_map(&vec, mapfunc);

  for (int i = 0; i < mapped.len; ++i) {
    printf("%s\n", (char*)stdvec_at(&mapped, i));
  }

  stdvec_free(&mapped);
  return 0;
}
