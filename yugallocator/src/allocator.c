#include "allocator.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

static void split_block(BlockHeader *block, size_t size) {
  if (block->size >= size + HEADER_SIZE + ALIGNMENT) {
    BlockHeader *split = (BlockHeader *)((char *)block + HEADER_SIZE + size);
    split->size = block->size - size - HEADER_SIZE;
    split->free = 1;
    split->next = block->next;
    block->size = size;
    block->next = split;
  }
}

static ZoneType get_zone_type(size_t size) {
  if (size <= TINY_MAX)
    return ZONE_TINY;
  if (size <= SMALL_MAX)
    return ZONE_SMALL;
  return ZONE_LARGE;
}

static ZoneHeader *find_zone_for_ptr(void *ptr, ZoneType *type_out) {
  ZoneHeader *lists[3] = {tiny_zones, small_zones, large_zones};
  ZoneType types[3] = {ZONE_TINY, ZONE_SMALL, ZONE_LARGE};

  int i;
  for (i = 0; i < 3; i++) {
    ZoneHeader *zone = lists[i];
    while (zone != NULL) {
      char *start = (char *)zone;
      char *end = start + zone->size;
      if ((char *)ptr >= start && (char *)ptr < end) {
        *type_out = types[i];
        return zone;
      }
      zone = zone->next;
    }
  }
  return NULL;
}

static void coalesce(ZoneHeader *zone) {
  BlockHeader *current = (BlockHeader *)((char *)zone + ZONE_HEADER_SIZE);
  while ((char *)current < (char *)zone + zone->size) {
    BlockHeader *next =
        (BlockHeader *)((char *)current + HEADER_SIZE + current->size);
    if ((char *)next >= (char *)zone + zone->size)
      break;
    if (current->free && next->free) {
      current->size += HEADER_SIZE + next->size;
      current->next = next->next;
    } else {
      current = next;
    }
  }
}

void *my_malloc(size_t size) {
  if (size == 0)
    return NULL;

  size = ALIGN(size);

  ZoneType type = get_zone_type(size);
  ZoneHeader **list;

  if (type == ZONE_TINY) {
    list = &tiny_zones;
  } else if (type == ZONE_SMALL) {
    list = &small_zones;
  } else {
    list = &large_zones;
  }

  if (type != ZONE_LARGE) {
    BlockHeader *block = find_free_block(*list, size);

    if (block != NULL) {
      split_block(block, size);
      block->free = 0;
      return (void *)((char *)block + HEADER_SIZE);
    }
  }

  ZoneHeader *zone = new_zone(type, size);
  if (zone == NULL) {
    return NULL;
  }
  add_zone(type, zone);

  if (type == ZONE_LARGE) {
    BlockHeader *block = (BlockHeader *)((char *)zone + ZONE_HEADER_SIZE);
    block->free = 0;
    return (void *)((char *)block + HEADER_SIZE);
  }

  BlockHeader *block = find_free_block(zone, size);
  if (block == NULL)
    return NULL;
  split_block(block, size);
  block->free = 0;
  return (void *)((char *)block + HEADER_SIZE);
}

void my_free(void *ptr) {
  if (ptr == NULL)
    return;

  ZoneType type;
  ZoneHeader *zone = find_zone_for_ptr(ptr, &type);

  if (zone == NULL)
    return;

  BlockHeader *block = (BlockHeader *)((char *)ptr - HEADER_SIZE);
  block->free = 1;

  if (type != ZONE_LARGE)
    coalesce(zone);

  if (zone_is_empty(zone))
    remove_zone(type, zone);
}

void *my_realloc(void *ptr, size_t size) {
  if (ptr == NULL)
    return NULL;

  if (size == 0) {
    my_free(ptr);
    return NULL;
  }
  size = ALIGN(size);

  BlockHeader *block = (BlockHeader *)((char *)ptr - HEADER_SIZE);

  if (block->size >= size) {
    return ptr;
  }

  void *new_ptr = my_malloc(size);

  if (new_ptr == NULL)
    return NULL;

  memcpy(new_ptr, ptr, block->size);
  my_free(ptr);

  return new_ptr;
}
