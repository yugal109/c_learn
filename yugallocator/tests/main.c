#include "allocator.h"
#include <stdio.h>
#include <string.h>

void test_tiny(void) {
  printf("=== Tiny allocations (≤ 128 bytes) ===\n");

  int *a = (int *)my_malloc(sizeof(int));
  *a = 42;
  printf("a = %d\n", *a);

  char *str = (char *)my_malloc(64);
  strcpy(str, "hello yugalloc");
  printf("str = %s\n", str);

  double *d = (double *)my_malloc(sizeof(double));
  *d = 3.14;
  printf("d = %.2f\n", *d);

  show_alloc_mem();
  my_malloc_dump();

  my_free(a);
  my_free(str);
  my_free(d);

  printf("after freeing all — zone should be munmapped\n");
  my_malloc_dump();
  printf("\n");
}

void test_small(void) {
  printf("=== Small allocations (≤ 1024 bytes) ===\n");

  char *buf1 = (char *)my_malloc(256);
  char *buf2 = (char *)my_malloc(512);
  char *buf3 = (char *)my_malloc(1024);

  strcpy(buf1, "small buffer 1");
  strcpy(buf2, "small buffer 2");
  strcpy(buf3, "small buffer 3");

  printf("buf1 = %s\n", buf1);
  printf("buf2 = %s\n", buf2);
  printf("buf3 = %s\n", buf3);

  show_alloc_mem();

  my_free(buf1);
  my_free(buf2);
  my_free(buf3);

  my_malloc_dump();
  printf("\n");
}

void test_large(void) {
  printf("=== Large allocations (> 1024 bytes) ===\n");

  char *big1 = (char *)my_malloc(2000);
  char *big2 = (char *)my_malloc(1024 * 1024);

  strcpy(big1, "large allocation 1");
  strcpy(big2, "large allocation 2");

  printf("big1 = %s\n", big1);
  printf("big2 = %s\n", big2);

  show_alloc_mem();

  my_free(big1);
  my_free(big2);

  printf("after freeing — large zones munmapped\n");
  my_malloc_dump();
  printf("\n");
}

void test_calloc(void) {
  printf("=== Calloc test ===\n");

  int *arr = (int *)my_calloc(10, sizeof(int));

  int i;
  printf("calloc'd array (should all be 0): ");
  for (i = 0; i < 10; i++)
    printf("%d ", arr[i]);
  printf("\n");

  my_free(arr);
  printf("\n");
}

void test_realloc(void) {
  printf("=== Realloc test ===\n");

  char *str = (char *)my_malloc(10);
  strcpy(str, "hello");
  printf("before: %s at %p\n", str, (void *)str);

  str = (char *)my_realloc(str, 100);
  printf("after grow: %s at %p\n", str, (void *)str);

  str = (char *)my_realloc(str, 5);
  printf("after shrink: %s at %p\n", str, (void *)str);

  my_free(str);
  printf("\n");
}

void test_reallocf(void) {
  printf("=== Reallocf test ===\n");

  char *str = (char *)my_malloc(10);
  strcpy(str, "hello");
  printf("before: %s\n", str);

  str = (char *)my_reallocf(str, 100);
  if (str != NULL) {
    printf("after reallocf: %s\n", str);
    my_free(str);
  }
  printf("\n");
}

void test_coalescing(void) {
  printf("=== Coalescing test ===\n");

  void *p1 = my_malloc(32);
  void *p2 = my_malloc(32);
  void *p3 = my_malloc(32);

  my_malloc_dump();

  printf("freeing all...\n");
  my_free(p1);
  my_free(p2);
  my_free(p3);

  printf("after coalescing — zone munmapped:\n");
  my_malloc_dump();
  printf("\n");
}

void test_zone_reuse(void) {
  printf("=== Zone reuse test ===\n");

  void *p1 = my_malloc(64);
  void *p2 = my_malloc(64);
  void *p3 = my_malloc(64);

  printf("p1=%p p2=%p p3=%p\n", p1, p2, p3);

  my_free(p2);
  my_malloc_dump();

  void *p4 = my_malloc(64);
  printf("p4=%p (should reuse p2's block)\n", p4);

  my_free(p1);
  my_free(p3);
  my_free(p4);
  printf("\n");
}

void test_stress(void) {
  printf("=== Stress test ===\n");

  void *ptrs[200];
  int i;

  for (i = 0; i < 100; i++)
    ptrs[i] = my_malloc(64);

  for (i = 0; i < 100; i++)
    ptrs[100 + i] = my_malloc(512);

  for (i = 0; i < 100; i += 2)
    my_free(ptrs[i]);

  for (i = 0; i < 100; i += 2)
    ptrs[i] = my_malloc(64);

  for (i = 0; i < 200; i++)
    my_free(ptrs[i]);

  printf("stress test done — all zones should be empty\n");
  my_malloc_dump();
  printf("\n");
}

int main(void) {
  test_tiny();
  test_small();
  test_large();
  test_calloc();
  test_realloc();
  test_reallocf();
  test_coalescing();
  test_zone_reuse();
  test_stress();

  printf("all tests done.\n");
  return 0;
}
