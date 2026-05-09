#include "allocator.h"
#include <string.h>

// doesn't free the orignal pointer
void *my_calloc(size_t count, size_t size) {

  if (count == 0 || size == 0)
    return NULL;

  size_t total = count * size;
  void *ptr = my_malloc(total);

  if (ptr == NULL)
    return NULL;

  memset(ptr, 0, total);
  return ptr;
}

// frees the orignal pointer
void *my_reallocf(void *ptr, size_t size) {
  void *new_ptr = my_realloc(ptr, size);

  if (new_ptr == NULL && size != 0) {
    my_free(ptr);
    return NULL;
  }
  return new_ptr;
}
