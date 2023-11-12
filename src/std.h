#ifndef STD_H
#define STD_H

#include <aio.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __STD_PANIC(msg, ...)                                           \
  do {                                                                  \
    fprintf(stderr, "[PANIC: %s]: " msg "\n", __func__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);                                                 \
  } while (0)

#define __STD_DA_APPEND(da, dadata, dastride, dalen, dacap, value)      \
  do {                                                                  \
    if ((da)->dalen >= (da)->dacap) {                                   \
      (da)->dacap *= 2;                                                 \
      (da)->dadata = realloc((da)->dadata, (da)->dacap*(da)->dastride); \
    }                                                                   \
    (void)memcpy((da)->dadata+(da)->dalen*(da)->dastride, (value), (da)->dastride); \
    (da)->dalen += 1;                                                   \
  } while (0)                                                           \

#define __STD_CHECK_MEM(m)                              \
  do {                                                  \
    if ((m) == NULL) __STD_PANIC("invalid memory");     \
  } while (0)

// Compound Literal.
#define STDCL(type, x) ((void*)&(type){(x)})

//////////////////////////////
// StdVec IMPLEMENTATION
#ifdef STDVEC_IMPL

struct StdVec
{
  void *data;
  size_t stride;
  size_t len;
  size_t cap;
};
typedef struct StdVec StdVec;

StdVec
stdvec_create(size_t stride)
{
  void *data = malloc(stride);
  if (!data) {
    __STD_PANIC("could not allocate %zu bytes", stride);
  }
  return (StdVec) {
    .data = data,
    .cap = 1,
    .len = 0,
    .stride = stride,
  };
}

StdVec
stdvec_wcap(size_t stride, size_t cap)
{
  size_t bytes = stride*cap;
  void *data = malloc(bytes);
  if (!data) {
    __STD_PANIC("could not allocate %zu bytes", bytes);
  }
  return (StdVec) {
    .data = data,
    .cap = cap,
    .len = 0,
    .stride = stride,
  };
}

void
stdvec_push(StdVec *stdvec, void *value)
{
  __STD_CHECK_MEM(stdvec->data);
  __STD_DA_APPEND(stdvec, data, stride, len, cap, value);
}

void *
stdvec_at(StdVec *stdvec, size_t i)
{
  __STD_CHECK_MEM(stdvec->data);
  return stdvec->data+i*stdvec->stride;
}

void
stdvec_rm_at(StdVec *stdvec, size_t idx)
{
  __STD_CHECK_MEM(stdvec->data);
  memmove(stdvec_at(stdvec, idx), stdvec_at(stdvec, idx+1), (stdvec->len-idx-1)*stdvec->stride);
  stdvec->len--;
}

void
stdvec_rm(StdVec *stdvec, void *elem)
{
  __STD_CHECK_MEM(stdvec->data);
  for (size_t i = 0; i < stdvec->len; ++i) {
    if (memcmp(stdvec_at(stdvec, i), elem, stdvec->stride) == 0) {
      stdvec_rm_at(stdvec, i);
    }
  }
}

void *
stdvec_contains(StdVec *stdvec, void *elem)
{
  __STD_CHECK_MEM(stdvec->data);
  for (size_t i = 0; i < stdvec->len; ++i) {
    if (memcmp(stdvec_at(stdvec, i), elem, stdvec->stride) == 0) {
      return stdvec->data+i*stdvec->stride;
    }
  }
  return NULL;
}

void
stdvec_clr(StdVec *stdvec)
{
  stdvec->len = 0;
}

void
stdvec_free(StdVec *stdvec)
{
  __STD_CHECK_MEM(stdvec->data);
  free(stdvec->data);
  stdvec->data = NULL;
  stdvec->len = stdvec->cap = stdvec->stride = 0;
}

StdVec
stdvec_map(StdVec *stdvec, void (*mapfunc)(void *))
{
  StdVec mapped = stdvec_create(stdvec->stride);
  for (size_t i = 0; i < stdvec->len; i++) {
    mapfunc(stdvec_at(stdvec, i));
    stdvec_push(&mapped, stdvec_at(stdvec, i));
  }
  stdvec_free(stdvec);
  return mapped;
}

void
stdvec_qsort(StdVec *stdvec, int (*compar)(const void *, const void *))
{
  qsort(stdvec->data, stdvec->len, stdvec->stride, compar);
}

// TODO: optimize
void
stdvec_rev(StdVec *stdvec)
{
  int start = 0, end = stdvec->len-1;
  char tmp[stdvec->stride];
  while (start < end) {
    memcpy(tmp, stdvec->data + start * stdvec->stride, stdvec->stride);
    memcpy(stdvec->data + start * stdvec->stride, stdvec->data + end * stdvec->stride,
           stdvec->stride);
    memcpy(stdvec->data + end * stdvec->stride, tmp, stdvec->stride);
    start += 1;
    end -= 1;
  }
}
#endif // STDVEC_IMPL

//////////////////////////////
// StdUMap IMPLEMENTATION
#ifdef STDUMAP_IMPL
struct __StdUMapNode
{
  struct __StdUMapNode *next;
  void *key;
  void *value;
  size_t key_stride;
  size_t value_stride;
};
typedef struct __StdUMapNode __StdUMapNode;

struct StdUMap
{
  __StdUMapNode **table;
  unsigned int (*hashfunc)(const void *);
  size_t key_stride;
  size_t value_stride;
  size_t len;
  size_t cap;
};
typedef struct StdUMap StdUMap;

StdUMap
stdumap_create(size_t key_stride,
               size_t value_stride,
               unsigned int (*hashfunc)(const void *))
{
  const size_t cap = 10;
  __StdUMapNode **table = malloc(sizeof(__StdUMapNode)*cap);
  memset(table, 0, sizeof(__StdUMapNode)*cap);
  return (StdUMap) {
    .table = table,
    .hashfunc = hashfunc,
    .len = 0,
    .cap = cap,
    .key_stride = key_stride,
    .value_stride = value_stride,
  };
}

void
stdumap_free(StdUMap *stdumap)
{
  for (size_t i = 0; i < stdumap->cap; ++i) {
    __StdUMapNode *tmp = stdumap->table[i];
    while (tmp) {
      __StdUMapNode *next = tmp->next;
      free(tmp);
      tmp = next;
    }
  }
  free(stdumap->table);
  stdumap->table = NULL;
  stdumap->len = stdumap->cap = 0;
  stdumap->key_stride = stdumap->value_stride = 0;
}

__StdUMapNode *
__stdumapnode_alloc(void *key,
                    void *value,
                    size_t key_stride,
                    size_t value_stride,
                    __StdUMapNode *next)
{
  __StdUMapNode *node = malloc(sizeof(__StdUMapNode));
  node->next = next;
  node->key = key;
  node->value = value;
  node->key_stride = key_stride;
  node->value_stride = value_stride;
  return node;
}

void
__stdumap_resize(StdUMap *stdumap)
{
  size_t new_cap = stdumap->cap*2;
  __StdUMapNode **new_table = malloc(sizeof(__StdUMapNode*)*new_cap);

  for (size_t i = 0; i < stdumap->len; ++i) {
    __StdUMapNode *tmp = stdumap->table[i];
    while (tmp) {
      __StdUMapNode *next = tmp->next;
      unsigned int new_idx = stdumap->hashfunc(tmp->key) % new_cap;
      tmp->next = new_table[new_idx];
      new_table[new_idx] = tmp;
      tmp = next;
    }
  }

  free(stdumap->table);
  stdumap->table = new_table;
  stdumap->cap = new_cap;
}

__StdUMapNode *
__stdumap_find(__StdUMapNode *lst, void *key, __StdUMapNode **parent)
{
  __StdUMapNode *it = lst;

  if (parent) {
    *parent = NULL;
  }

  while (it && (memcmp(it->key, key, it->key_stride) != 0)) {
    if (parent) {
      *parent = it;
    }
    it = it->next;
  }

  return it;
}

void
stdumap_insert(StdUMap *stdumap, void *key, void *value)
{
  unsigned int idx = stdumap->hashfunc(key) % stdumap->cap;
  __StdUMapNode *p = __stdumap_find(stdumap->table[idx], key, NULL);

  if (!p) {
    stdumap->table[idx] =
      __stdumapnode_alloc(key, value,
                          stdumap->key_stride,
                          stdumap->value_stride,
                          stdumap->table[idx]);
    memcpy(stdumap->table[idx]->key, key, stdumap->key_stride);
    memcpy(stdumap->table[idx]->value, value, stdumap->value_stride);
    stdumap->len++;
  }
  else {
    memcpy(p->value, value, stdumap->value_stride);
  }

  if (((float)stdumap->len / stdumap->cap) > 0.5f) {
    __stdumap_resize(stdumap);
  }
}

void *
stdumap_get(StdUMap *stdumap, void *key)
{
  unsigned int idx = stdumap->hashfunc(key) % stdumap->cap;
  __StdUMapNode *p = __stdumap_find(stdumap->table[idx], key, NULL);
  return p ? p->value : NULL;
}

#endif // STDUMAP_IMPL
#endif // STD_H
