// MIT License

// Copyright (c) 2023 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef STD_H
#define STD_H

#include <aio.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOTE: all functions/macros starting with two
// underscores `__` are to be treated as private.

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
stdvec_new(size_t stride)
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
      --i;
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
  StdVec mapped = stdvec_new(stdvec->stride);
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

// Private function to shift all elements left in
// `str` from `start` -> `end`.
void
__stdstr_shift_elems_left(StdStr *str, size_t start, size_t end)
{
  if (start > str->len) {
    __STD_PANIC("index %zu is out of bounds of length %zu", start, str->len);
  }
  if (end > str->len) {
    __STD_PANIC("index %zu is out of bounds of length %zu", end, str->len);
  }
  for (size_t i = start; i < end-1; ++i) {
    str->data[i] = str->data[i+1];
  }
}

// Remove a char at `idx`. This function decrements
// the length of `str`.
void
stdstr_rm_at(StdStr *str, size_t idx)
{
  if (idx >= str->len) {
    __STD_PANIC("index %zu is out of bounds of length %zu", idx, str->len);
  }
  __stdstr_shift_elems_left(str, idx, str->len);
  str->len--;
}

// Remove all occurences of `value` in `str`.
// This function does not decrement `len` since
// it calls stdstr_rm_at which does decrement it.
void
stdstr_rmchar(StdStr *str, char value)
{
  for (size_t i = 0; i < str->len; ++i) {
    if (str->data[i] == value) {
      stdstr_rm_at(str, i);
      i--;
    }
  }
}

#endif // STDSTR_IMPL

//////////////////////////////
// Functions IMPLEMENTATION
#ifdef STDFUNCS_IMPL

// Returns an integer that increments whenever
// the function gets called.
int
stdiota(void)
{
  static int __iota_incrementor = 0;
  return __iota_incrementor++;
}

// Fill an array with stdiota.
void
stdiota_fill(int *arr, size_t len)
{
  for (size_t i = 0; i < len; ++i) {
    arr[i] = stdiota();
  }
}

// Returns 1 if all elements in `arr` satisfy
// the function `boolfunc`. Otherwise it
// returns 0.
int
stdall_of(void *arr, size_t stride, size_t len, int (boolfunc)(const void *))
{
  for (size_t i = 0; i < len; ++i) {
    if (!boolfunc(arr+i*stride)) {
      return 0;
    }
  }
  return 1;
}

// Returns 1 if any elements in `arr` satisfy
// the function `boolfunc`. Otherwise it
// returns 0.
int
stdany_of(void *arr, size_t stride, size_t len, int (boolfunc)(const void *))
{
  for (size_t i = 0; i < len; ++i) {
    if (boolfunc(arr+i*stride)) {
      return 1;
    }
  }
  return 0;
}

// Returns 1 if none of the elements in `arr` satisfy
// the function `boolfunc`. Otherwise it
// returns 0.
int
stdnone_of(void *arr, size_t stride, size_t len, int (boolfunc)(const void *))
{
  for (size_t i = 0; i < len; ++i) {
    if (boolfunc(arr+i*stride)) {
      return 0;
    }
  }
  return 1;
}

// Swaps element a with element b
// complains if either is null for some reason
#define __STDSWAP(type)                     \
        void stdswap_##type(type* _val1, type* _val2) \
        {                                             \
          if (_val1 == NULL || _val2 == NULL)         \
            __STD_PANIC("null pointer detected in stdswap_%s", #type); \
          type tmp = *_val1;                          \
          *_val1 = *_val2;                            \
          *_val2 = tmp;                               \
        }


typedef int (*CompareFunction)(const void *, const void *);

const void*
std_is_sorted_until(const void *first, const void *last, size_t stride, CompareFunction compare)
{
  if (first != last) {
    const void *next = first;
    while ((next = (const char *)next + stride) != last) {
      if (compare(next,first) < 0) {
        return next;
      }
      first = next;
    }
  }
  return last;
}

int
std_is_sorted(const void *first, const void *last, size_t stride, CompareFunction compare)
{
  return std_is_sorted_until(first, last, stride, compare) == last;
}



#endif // STDFUNCS_IMPL

//////////////////////////////
// StdStack IMPLEMENTATION
#ifdef STDSTACK_IMPL

struct StdStack
{
  void *data;
  size_t stride;
  size_t len;
  size_t cap;
};
typedef struct StdStack StdStack;

// Create a new StdStack with element
// size being `stride`.
StdStack
stdstack_new(size_t stride)
{
  StdStack stack;
  stack.data   = __STD_S_MALLOC(stride);
  stack.cap    = 1;
  stack.len    = 0;
  stack.stride = stride;
  return stack;
}

// Free the underlying memory of `stack`.
void
stdstack_free(StdStack *stack)
{
  __STD_CHECK_MEM(stack->data);
  free(stack->data);
  stack->data = NULL;
  stack->len = stack->cap = stack->stride = 0;
}

// Returns 1 if stack is empty, 0 otherwise.
int
stdstack_empty(StdStack *stack)
{
  return stack->len == 0;
}

// Get the element at the end of `stack`
// i.e., stack->len-1.
void *
stdstack_peek(StdStack *stack)
{
  return stack->len == 0
    ? NULL
    : stack->data+(stack->len-1)*stack->stride;
}

// Remove the value at the end of `stack`.
// Panics if len = 0.
void
stdstack_pop(StdStack *stack)
{
  if (stack->len == 0) {
    __STD_PANIC("tried to pop element of a stack but its len = 0");
  }
  stack->len--;
}

// Push an element into `stack` at the end.
void
stdstack_push(StdStack *stack, void *value)
{
  __STD_CHECK_MEM(stack->data);
  __STD_DA_APPEND(stack, data, stride, len, cap, value);
}

#endif // STDSTACK_IMPL

//////////////////////////////
// StdPair IMPLEMENTATION
#ifdef STDPAIR_IMPL

struct StdPair
{
  void *fst;
  void *snd;
};
typedef struct StdPair StdPair;

// Create a new StdPair.
StdPair
stdpair_new(void *fst, void *snd)
{
  StdPair pair;
  memcpy(&pair.fst, &fst, sizeof(fst));
  memcpy(&pair.snd, &snd, sizeof(snd));
  return pair;
}

// Get the first element of `pair`.
void *
stdpair_fst(StdPair *pair)
{
  return pair->fst;
}

// Get the second element of `pair`.
void *
stdpair_snd(StdPair *pair)
{
  return pair->snd;
}

#endif // STDPAIR_IMPL

////////////////////////////////
// StdQueue IMPLEMENTATION
#ifdef STDQUEUE_IMPL

struct StdQueue
{
  void *data;
  size_t stride;
  size_t len;
  size_t cap;
  size_t head;  // Index of the front of the queue
};
typedef struct StdQueue StdQueue;

// Create a new StdQueue with element
// size being `stride`.
StdQueue
stdqueue_new(size_t stride)
{
  StdQueue queue;
  queue.data   = __STD_S_MALLOC(stride);
  queue.cap    = 1;
  queue.len    = 0;
  queue.head   = 0;
  queue.stride = stride;
  return queue;
}

// Free the underlying memory of `queue`.
void
stdqueue_free(StdQueue *queue)
{
  __STD_CHECK_MEM(queue->data);
  free(queue->data);
  queue->data = NULL;
  queue->len = queue->cap = queue->stride = queue->head = 0;
}

// Returns 1 if queue is empty, 0 otherwise.
int
stdqueue_empty(StdQueue *queue)
{
  return queue->len == 0;
}

// Get the element at the front of `queue`.
// Returns NULL if the queue is empty.
void *
stdqueue_peek(StdQueue *queue)
{
  return queue->len == 0
    ? NULL
    : queue->data + queue->head * queue->stride;
}

// Remove the value at the front of `queue`.
// Panics if len = 0.
void
stdqueue_dequeue(StdQueue *queue)
{
  if (queue->len == 0) {
    __STD_PANIC("tried to dequeue from an empty queue");
  }
  queue->head = (queue->head + 1) % queue->cap;
  queue->len--;
}

// Enqueue an element into `queue` at the end.
void
stdqueue_enqueue(StdQueue *queue, void *value)
{
  __STD_CHECK_MEM(queue->data);
  __STD_DA_APPEND(queue, data, stride, len, cap, value);
}

#endif // STDQUEUE_IMPL

#endif // STD_H
