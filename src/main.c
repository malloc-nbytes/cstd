#define STDUMAP_IMPL
#define STDVEC_IMPL
#include "./std.h"
#include <stdio.h>
#include <limits.h>

unsigned int hashfunc(const void *k) {
  return *(int*)k % INT_MAX;
}

int
main(void)
{
  StdUMap m = stdumap_create(sizeof(int), sizeof(int), hashfunc);

  for (int i = 0; i < 10; ++i) {
    stdumap_insert(&m, &i, &i);
  }

  for (int i = 0; i < m.len; ++i) {
    int *k = stdumap_get(&m, &i);
    printf("%d\n", *k);
  }

  stdumap_free(&m);

  return 0;
}
