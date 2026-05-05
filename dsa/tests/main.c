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
    printf("arr[%d] = %d\n", i, *val);
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
  array_get(arr, 100);

  array_free(arr, NULL);
  printf("array freed\n\n");
}

void test_list(void) {
  printf("=== List Tests ===\n");

  List *list = list_create();

  int a = 1, b = 2, c = 3, d = 4;

  list_push_back(list, &a);
  list_push_back(list, &b);
  list_push_back(list, &c);
  list_push_front(list, &d);

  printf("size: %d\n", list_size(list));

  int i;
  for (i = 0; i < list_size(list); i++) {
    int *val = (int *)list_get(list, i);
    printf("list[%d] = %d\n", i, *val);
  }

  int *popped = (int *)list_pop_front(list);
  printf("popped front: %d\n", *popped);
  printf("size after pop: %d\n", list_size(list));

  printf("out of bounds test: ");
  list_get(list, 100);

  list_free(list, NULL);
  printf("list freed\n\n");
}

void test_hashmap(void) {
  printf("=== Hashmap Tests ===\n");

  Hashmap *map = hashmap_create();

  int age1 = 25, age2 = 30, age3 = 22;

  hashmap_put(map, "alice", &age1);
  hashmap_put(map, "bob", &age2);
  hashmap_put(map, "carol", &age3);

  printf("size: %d\n", hashmap_size(map));

  int *age = (int *)hashmap_get(map, "alice");
  printf("alice: %d\n", *age);

  age = (int *)hashmap_get(map, "bob");
  printf("bob: %d\n", *age);

  age = (int *)hashmap_get(map, "carol");
  printf("carol: %d\n", *age);

  int age4 = 99;
  hashmap_put(map, "alice", &age4);
  age = (int *)hashmap_get(map, "alice");
  printf("alice after update: %d\n", *age);

  hashmap_remove(map, "bob", NULL);
  printf("size after removing bob: %d\n", hashmap_size(map));

  void *result = hashmap_get(map, "bob");
  printf("bob after remove: %s\n", result == NULL ? "NULL" : "found");

  result = hashmap_get(map, "nobody");
  printf("nobody: %s\n", result == NULL ? "NULL" : "found");

  hashmap_free(map, NULL);
  printf("hashmap freed\n\n");
}

void test_hashmap_resize(void) {
  printf("=== Hashmap Resize Test ===\n");

  Hashmap *map = hashmap_create();

  int values[20];
  char key[16];
  int i;

  for (i = 0; i < 20; i++) {
    values[i] = i * 10;
    snprintf(key, sizeof(key), "key%d", i);
    hashmap_put(map, key, &values[i]);
  }

  printf("size after 20 inserts: %d\n", hashmap_size(map));

  for (i = 0; i < 20; i++) {
    snprintf(key, sizeof(key), "key%d", i);
    int *val = (int *)hashmap_get(map, key);
    printf("%s = %d\n", key, *val);
  }

  hashmap_free(map, NULL);
  printf("hashmap freed after resize test\n\n");
}

int main(void) {
  test_array();
  test_list();
  test_hashmap();
  test_hashmap_resize();

  printf("all tests passed.\n");
  return 0;
}
