#ifndef STD_H
#define STD_H

#include <aio.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A variadic panic out and exit with a message.
// Example usage:
//   int num;
//   __STD_PANIC("got num: %d", num);
#define __STD_PANIC(msg, ...)                                           \
  do {                                                                  \
    fprintf(stderr, "[PANIC: %s]: " msg "\n", __func__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);                                                 \
  } while (0)

// Generic dynamic array append macro. It should be used for
// structures such as:
// struct {
//   pointer;
//   number;
//   number;
//   number;
// }
// The names may be different, which is why we have the params of
// dadata, dastride, dalen, dacap, value.
// Example usage:
//   struct Vec {
//     void *data;
//     size_t stride, len, cap;
//   };
//   Vec v;
//   __STD_DA_APPEND(&v, data, stride, len, cap);
#define __STD_DA_APPEND(da, dadata, dastride, dalen, dacap, value)      \
  do {                                                                  \
    if ((da)->dalen >= (da)->dacap) {                                   \
      (da)->dacap *= 2;                                                 \
      (da)->dadata = realloc((da)->dadata, (da)->dacap*(da)->dastride); \
    }                                                                   \
    (void)memcpy((da)->dadata+(da)->dalen*(da)->dastride, (value), (da)->dastride); \
    (da)->dalen += 1;                                                   \
  } while (0)                                                           \

// Check if memory is valid.
#define __STD_CHECK_MEM(m)                      \
  do {                                          \
    if ((m) == NULL)  {                         \
      __STD_PANIC("invalid memory");            \
    }                                           \
  } while (0)

// Safe malloc() wrapper so we don't have to
// keep checking if malloc() failed or not.
// Example usage:
//   int *ptr = __STD_S_MALLOC(sizeof(int));
#define __STD_S_MALLOC(bytes) ({                        \
      void *p = malloc(bytes);                          \
      if (!p) {                                         \
        fprintf(stderr, "Memory allocation failed.\n"); \
        exit(EXIT_FAILURE);                             \
      }                                                 \
      p;                                                \
    })

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

// Creates a new StdVec. It sets the element size
// to `stride`, and allocates `stride` bytes of memory.
StdVec
stdvec_create(size_t stride)
{
  void *data = __STD_S_MALLOC(stride);
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

// Create a new stdvec with a predefined capacity.
StdVec
stdvec_wcap(size_t stride, size_t cap)
{
  size_t bytes = stride*cap;
  void *data = __STD_S_MALLOC(bytes);
  return (StdVec) {
    .data = data,
    .cap = cap,
    .len = 0,
    .stride = stride,
  };
}

// Push a value into the end of the stdvec.
void
stdvec_push(StdVec *stdvec, void *value)
{
  __STD_CHECK_MEM(stdvec->data);
  __STD_DA_APPEND(stdvec, data, stride, len, cap, value);
}

// Return the value at a specific index.
void *
stdvec_at(StdVec *stdvec, size_t i)
{
  __STD_CHECK_MEM(stdvec->data);
  return stdvec->data+i*stdvec->stride;
}

// Remove the value at a specific index.
void
stdvec_rm_at(StdVec *stdvec, size_t idx)
{
  __STD_CHECK_MEM(stdvec->data);
  memmove(stdvec_at(stdvec, idx), stdvec_at(stdvec, idx+1), (stdvec->len-idx-1)*stdvec->stride);
  stdvec->len--;
}

// Remove all occurrences of elem.
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

// Check to see if the stdvec contains elem.
// Returns a pointer to that element if it is present.
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

// Clear the stdvec.
void
stdvec_clr(StdVec *stdvec)
{
  stdvec->len = 0;
}

// Free underlying memory of the stdvec.
void
stdvec_free(StdVec *stdvec)
{
  __STD_CHECK_MEM(stdvec->data);
  free(stdvec->data);
  stdvec->data = NULL;
  stdvec->len = stdvec->cap = stdvec->stride = 0;
}

// Apply a mapping function to each element.
// Returns a new stdvec.
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

// Quicksort a stdvec.
void
stdvec_qsort(StdVec *stdvec, int (*compar)(const void *, const void *))
{
  qsort(stdvec->data, stdvec->len, stdvec->stride, compar);
}

// Reverse the contents of the stdvec.
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
// StdVec IMPLEMENTATION
#ifdef STDSTR_IMPL

// This implementation of a string
// does not use a null byte. It entirely
// depends on `len` and `cap`.
struct StdStr
{
  char *data;
  size_t len;
  size_t cap;
};
typedef struct StdStr StdStr;

// Creates a new stdstr. Allocates 1 byte
// of data for the null byte.
StdStr
stdstr_new(void)
{
  StdStr str;
  str.data = __STD_S_MALLOC(1);
  str.len = 0;
  str.cap = 1;
  return str;
}

// Pushes a char into the str.
// This function is different from stdstr_append
// as this only pushes a char.
// Will reallocate data if needed.
void
stdstr_push(StdStr *str, char c)
{
  __STD_CHECK_MEM(str->data);
  if (str->len >= str->cap) {
    str->cap *= 2;
    str->data = realloc(str->data, str->cap);
  }
  str->data[str->len++] = c;
}

// Appends a char * into the end of str.
// This function is different from stdstr_push
// as this appends a char * instead of a single char.
// It calls stdstr_push so no reallocation is needed.
void
stdstr_append(StdStr *str, char *value)
{
  for (size_t i = 0; value[i] != '\0'; ++i) {
    stdstr_push(str, value[i]);
  }
}

// Creates a new stdstr with data from `from`.
StdStr
stdstr_from(char *from)
{
  StdStr str = stdstr_new();
  stdstr_append(&str, from);
  return str;
}

// Clear the contents of the `str`.
// All that is needed is setting the
// `len` to 0.
void
stdstr_clr(StdStr *str)
{
  str->len = 0;
}

// Print the data instead of `str`.
// We need this because StdStr does
// not use a null byte.
void
stdstr_print(StdStr *str)
{
  for (size_t i = 0; i < str->len; ++i) {
    printf("%c", str->data[i]);
  }
}

// Free the underlying contents of `str`
// aka the `data`.
void
stdstr_free(StdStr *str)
{
  __STD_CHECK_MEM(str->data);
  free(str->data);
  str->data = NULL;
  str->len = str->cap = 0;
}

// Create a new stdstr from the contents
// of a file. Takes a const char * instead
// of FILE * to avoid dealing with who
// closes the FILE *.
StdStr
stdstr_from_file(const char *filepath)
{
  StdStr str = stdstr_new();
  char *buf = 0;
  long len;

  FILE *fp = fopen(filepath, "r");

  if (!fp) {
    __STD_PANIC("could not open %s because %s", filepath, strerror(errno));
  }

  // Get the size of the file and load
  // it into buf.
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  buf = __STD_S_MALLOC(len);
  fread(buf, 1, len, fp);

  for (long i = 0; i < len; ++i) {
    stdstr_push(&str, buf[i]);
  }

  free(buf);
  fclose(fp);

  return str;
}

#endif // STDSTR_IMPL
#endif // STD_H
