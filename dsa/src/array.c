#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8

Array *array_create(int initial_capacity)
{
  Array *arr = malloc(sizeof(Array));
  if (arr == NULL)
  {
    fprintf(stderr, "array: memory allocation failed.\n");
    exit(1);
  }
  arr->capacity = initial_capacity > 0 ? initial_capacity : INITIAL_CAPACITY;
  arr->size = 0;
  arr->data = malloc(arr->capacity * sizeof(void *));

  if (arr->data == NULL)
  {
    fprintf(stderr, "array: memeory allocation failed.\n");
    exit(1);
  }
  return arr;
}

void array_push(Array *arr, void *item)
{
  if (arr->size >= arr->capacity)
  {
    arr->capacity *= 2;
    arr->data = realloc(arr->data, arr->capacity * sizeof(void *));
    if (arr->data == NULL)
    {
      fprintf(stderr, "array: memeory allocation failed.\n");
      exit(1);
    }
  }
  arr->data[arr->size] = item;
  arr->size++;
}

void *array_get(Array *arr, int index)
{
  if (index < 0 || index >= arr->size)
  {
    fprintf(stderr, "index %d out of bounds.\n", index);
    return NULL;
  }
  return arr->data[index];
}

void array_set(Array *arr, int index, void *item)
{
  if (index < 0 || index >= arr->size)
  {
    fprintf(stderr, "index %d out of bounds.\n", index);
    return;
  }
  arr->data[index] = item;
}

void array_remove(Array *arr, int index)
{
  if (index < 0 || index >= arr->size)
  {
    fprintf(stderr, "index %d out of bounds.\n", index);
    return;
  }
  memmove(arr->data + index, arr->data + index + 1,
          (arr->size - index - 1) * sizeof(void *));
  arr->size--;
}

int array_size(Array *arr) { return arr->size; }

void array_free(Array *arr, void (*free_item)(void *))
{
  if (free_item != NULL)
  {
    int i;
    for (i = 0; i < arr->size; i++)
    {
      free_item(arr->data[i]);
    }
  }
  free(arr->data);
  free(arr);
}
