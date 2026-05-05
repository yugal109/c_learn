#ifndef ARRAY_H
#define ARRAY_H

typedef struct {
  void **data;
  int size;
  int capacity;
} Array;

Array *array_create(int initial_capacity);
void array_push(Array *arr, void *item);
void *array_get(Array *arr, int index);
void array_set(Array *arr, int index, void *item);
void array_remove(Array *arr, int index);
int array_size(Array *arr);
void array_free(Array *arr, void (*free_item)(void *));

#endif
