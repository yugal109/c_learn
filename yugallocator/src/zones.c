#include "allocator.h"
#include <stdio.h>
#include <sys/mman.h>

ZoneHeader *tiny_zones = NULL;
ZoneHeader *small_zones = NULL;
ZoneHeader *large_zones = NULL;

static size_t get_zone_size(ZoneType type) {
  if (type == ZONE_TINY)
    return TINY_ZONE_SIZE;
  if (type == ZONE_SMALL)
    return SMALL_ZONE_SIZE;
  return 0;
}

static ZoneHeader **get_zone_list(ZoneType type) {
  if (type == ZONE_TINY)
    return &tiny_zones;
  if (type == ZONE_SMALL)
    return &small_zones;
  return &large_zones;
}

ZoneHeader *new_zone(ZoneType type, size_t alloc_size) {
  size_t zone_size;

  if (type == ZONE_LARGE)
    zone_size = ZONE_HEADER_SIZE + HEADER_SIZE + alloc_size;
  else
    zone_size = get_zone_size(type);

  void *mem = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (mem == MAP_FAILED)
    return NULL;

  ZoneHeader *zone = (ZoneHeader *)mem;
  zone->size = zone_size;
  zone->next = NULL;

  BlockHeader *block = (BlockHeader *)((char *)mem + ZONE_HEADER_SIZE);
  block->size = zone_size - ZONE_HEADER_SIZE - HEADER_SIZE;
  block->free = 1;
  block->next = NULL;

  return zone;
}

BlockHeader *find_free_block(ZoneHeader *zone, size_t size) {
  while (zone != NULL) {
    BlockHeader *block = (BlockHeader *)((char *)zone + ZONE_HEADER_SIZE);

    while ((char *)block < (char *)zone + zone->size) {
      if (block->free && block->size >= size) {
        return block;
      }
      block = (BlockHeader *)((char *)block + HEADER_SIZE + block->size);
    }
    zone = zone->next;
  }
  return NULL;
}

void add_zone(ZoneType type, ZoneHeader *zone) {
  ZoneHeader **list = get_zone_list(type);

  if (*list == NULL) {
    *list = zone;
    return;
  }

  ZoneHeader *current = *list;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = zone;
}

int zone_is_empty(ZoneHeader *zone) {
  BlockHeader *block = (BlockHeader *)((char *)zone + ZONE_HEADER_SIZE);
  while ((char *)block < (char *)zone + zone->size) {
    if (!block->free) {
      return 0;
    }
    block = (BlockHeader *)((char *)block + HEADER_SIZE + block->size);
  }
  return 1;
}

void remove_zone(ZoneType type, ZoneHeader *zone) {
  ZoneHeader **list = get_zone_list(type);
  ZoneHeader *current = *list;
  ZoneHeader *prev = NULL;

  while (current != NULL) {
    if (current == zone) {
      if (prev == NULL) {
        *list = current->next;
      } else {
        prev->next = current->next;
      }
      munmap(zone, zone->size);
      return;
    }
    prev = current;
    current = current->next;
  }
}
