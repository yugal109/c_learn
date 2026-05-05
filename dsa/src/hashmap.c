#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static unsigned int hash(const char *key, int capacity)
{
  unsigned int h = 5381;
  int c;
  while ((c = *key++))
    h = ((h << 5) + h) + c;
  return h % capacity;
}

static void hashmap_resize(Hashmap *map)
{
  int new_capacity = map->capacity * 2;
  Entry **new_buckets = calloc(new_capacity, sizeof(Entry *));
  if (new_buckets == NULL)
  {
    fprintf(stderr, "hashmap: memory allocation failed\n");
    exit(1);
  }
  int i;
  for (i = 0; i < map->capacity; i++)
  {
    Entry *entry = map->buckets[i];
    while (entry != NULL)
    {
      Entry *next = entry->next;
      unsigned int index = hash(entry->key, new_capacity);
      entry->next = new_buckets[index];
      new_buckets[index] = entry;
      entry = next;
    }
  }
  free(map->buckets);
  map->buckets = new_buckets;
  map->capacity = new_capacity;
}

Hashmap *hashmap_create(void)
{
  Hashmap *map = malloc(sizeof(Hashmap));
  if (map == NULL)
  {
    fprintf(stderr, "hashmap: memory allocation failed.\n");
    exit(1);
  }
  map->capacity = HASHMAP_INITIAL_CAPACITY;
  map->size = 0;
  map->buckets = calloc(map->capacity, sizeof(Entry *));

  if (map->buckets == NULL)
  {
    fprintf(stderr, "hashmap: memory allocation failed\n");
    exit(1);
  }
  return map;
}

void hashmap_put(Hashmap *map, const char *key, void *value)
{
  if ((float)map->size / map->capacity >= HASHMAP_LOAD_FACTOR)
  {
    hashmap_resize(map);
  }

  unsigned int index = hash(key, map->capacity);
  Entry *entry = map->buckets[index];

  while (entry != NULL)
  {
    if (strcmp(entry->key, key) == 0)
    {
      entry->value = value;
      return;
    }
    entry = entry->next;
  }

  Entry *new_entry = malloc(sizeof(Entry));
  if (new_entry == NULL)
  {
    fprintf(stderr, "hashmap: memory allocation failed\n");
    exit(1);
  }

  new_entry->key = strdup(key);
  new_entry->value = value;
  new_entry->next = map->buckets[index];
  map->buckets[index] = new_entry;
  map->size++;
}

void *hashmap_get(Hashmap *map, const char *key)
{
  unsigned int index = hash(key, map->capacity);
  Entry *entry = map->buckets[index];
  while (entry != NULL)
  {
    if (strcmp(entry->key, key) == 0)
    {
      return entry->value;
    }
    entry = entry->next;
  }
  return NULL;
}

void hashmap_remove(Hashmap *map, const char *key, void (*free_value)(void *))
{
  unsigned int index = hash(key, map->capacity);
  Entry *entry = map->buckets[index];
  Entry *prev = NULL;

  while (entry != NULL)
  {
    if (strcmp(entry->key, key) == 0)
    {
      if (prev == NULL)
      {
        map->buckets[index] = entry->next;
      }
      else
      {
        prev->next = entry->next;
      }
      if (free_value != NULL)
      {
        free_value(entry->value);
      }
      free(entry->key);
      free(entry);
      map->size--;
      return;
    }
    prev = entry;
    entry = entry->next;
  }
}

int hashmap_size(Hashmap *map) { return map->size; }

void hashmap_free(Hashmap *map, void (*free_value)(void *))
{
  int i;
  for (i = 0; i < map->capacity; i++)
  {
    Entry *entry = map->buckets[i];
    while (entry != NULL)
    {
      Entry *next = entry->next;
      if (free_value != NULL)
      {
        free_value(entry->value);
      }
      free(entry->key);
      free(entry);
      entry = next;
    }
  }
  free(map->buckets);
  free(map);
}
