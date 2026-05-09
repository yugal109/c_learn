#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

// size thresholds
#define TINY_MAX 128
#define SMALL_MAX 1024
#define MIN_ALLOCS 100

// alignment
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// block header - seits before every allocation
typedef struct BlockHeader {
  size_t size;
  int free;
  struct BlockHeader *next;
} BlockHeader;

#define HEADER_SIZE (ALIGN(sizeof(BlockHeader)))

// zone header - sitas at start of every mmap chunk
typedef struct ZoneHeader {
  size_t size;             // total size of this zone
  struct ZoneHeader *next; // next zome of same type
} ZoneHeader;

#define ZONE_HEADER_SIZE (ALIGN(sizeof(ZoneHeader)))

// zone sizes - enough for 100+ allocations each
#define TINY_ZONE_SIZE                                                         \
  (ZONE_HEADER_SIZE + (HEADER_SIZE + TINY_MAX) * MIN_ALLOCS)
#define SMALL_ZONE_SIZE                                                        \
  (ZONE_HEADER_SIZE + (HEADER_SIZE + SMALL_MAX) * MIN_ALLOCS)

// zone type
typedef enum { ZONE_TINY, ZONE_SMALL, ZONE_LARGE } ZoneType;

// global zone lists
extern ZoneHeader *tiny_zones;
extern ZoneHeader *small_zones;
extern ZoneHeader *large_zones;

// zone fucntions -defined in zones.c
ZoneHeader *new_zone(ZoneType type, size_t alloc_size);
BlockHeader *find_free_block(ZoneHeader *zone, size_t size);
void add_zone(ZoneType type, ZoneHeader *zone);
int zone_is_empty(ZoneHeader *zone);
void remove_zone(ZoneType type, ZoneHeader *zone);

// public Api
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);
void *my_calloc(size_t count, size_t size);
void *my_reallocf(void *ptr, size_t size);
void show_alloc_mem(void);
void my_malloc_dump(void);

#endif
