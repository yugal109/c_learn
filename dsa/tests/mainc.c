#include "array.h"
#include "hashmap.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_array(void) {
  printf("=== Array Tests ===\n");
  Array *arr = array_create(4);
  int a = 10, b = 20, c = 30, d = 40, e = 50;

  array_push(arr, &a);
  array_push(arr, &b);
  array_push(arr, &c);
  array_push(arr, &d);
  array_push(arr, &e);

  printf("size: %d\n", array_size(arr));

  int i;
  for (i = 0; i < array_size(arr); i++) {
    int *val = (int *)array_get(arr, i);
    printf("arr[%d] = %d\n", *val);
  }

  array_remove(arr, 2);
  printf("after removing index 2:\n");
  for (i = 0; i < array_size(arr); i++) {
    int *val = (int *)array_get(arr, i);
    printf("arr[%d] = %d\n", i, *val);
  }

  array_set(arr, 0, &e);
  printf("after setting index 0 to 50:\n");
  int *val = (int *)array_get(arr, 0);
  printf("arr[0] = %d\n", *val);

  printf("out of bounds test: ");
  array_get(arr, 10);

  array_free(arr, NULL);
  printf("array freed\n\n");
}

void test_list(void) {
  printf("=== list Test ===\n");

  List *list = list_create();
}
