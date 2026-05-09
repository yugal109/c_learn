#include "allocator.h"
#include <stdio.h>

static void print_zone(ZoneHeader *zone, const char *type) {
  while (zone != NULL) {
    printf("%s : %p\n", type, (void *)zone);

    BlockHeader *block = (BlockHeader *)((char *)zone + ZONE_HEADER_SIZE);
    size_t total_alloc = 0;

    while ((char *)block < (char *)zone + zone->size) {
      if (!block->free) {
        void *start = (void *)((char *)block + HEADER_SIZE);
        void *end = (void *)((char *)start + block->size);
        printf("%p - %p : %zu bytes\n", start, end, block->size);
        total_alloc += block->size;
      }
      block = (BlockHeader *)((char *)block + HEADER_SIZE + block->size);
    }

    printf("zone total allocated: %zu bytes\n", total_alloc);
    zone = zone->next;
  }
}

void show_alloc_mem(void) {
  printf("=== TINY zones ===\n");
  print_zone(tiny_zones, "TINY");

  printf("=== SMALL zones ===\n");
  print_zone(small_zones, "SMALL");

  printf("=== LARGE zones ===\n");
  print_zone(large_zones, "LARGE");
}

void my_malloc_dump(void) {
  printf("=== heap dump ===\n");

  ZoneHeader *lists[3] = {tiny_zones, small_zones, large_zones};
  const char *names[3] = {"TINY", "SMALL", "LARGE"};
  ZoneType types[3] = {ZONE_TINY, ZONE_SMALL, ZONE_LARGE};

  int i;
  for (i = 0; i < 3; i++) {
    ZoneHeader *zone = lists[i];
    int zone_num = 0;

    while (zone != NULL) {
      printf("[%s zone %d @ %p size=%zu]\n", names[i], zone_num, (void *)zone,
             zone->size);

      BlockHeader *block = (BlockHeader *)((char *)zone + ZONE_HEADER_SIZE);
      int block_num = 0;

      while ((char *)block < (char *)zone + zone->size) {
        printf("  block %d: size=%zu free=%d addr=%p\n", block_num, block->size,
               block->free, (void *)block);
        block_num++;
        block = (BlockHeader *)((char *)block + HEADER_SIZE + block->size);
      }

      zone_num++;
      zone = zone->next;
    }

    (void)types[i];
  }

  printf("=== end dump ===\n");
}
