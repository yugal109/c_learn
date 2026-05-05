#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_INITIAL_CAPACITY 16
#define HASHMAP_LOAD_FACTOR 0.75

typedef struct Entry {
  char *key;
  void *value;
  struct Entry *next;
} Entry;

typedef struct {
  Entry **buckets;
  int capacity;
  int size;
} Hashmap;

Hashmap *hashmap_create(void);
void hashmap_put(Hashmap *map, const char *key, void *value);
void *hashmap_get(Hashmap *map, const char *key);
void hashmap_remove(Hashmap *map, const char *key, void (*free_value)(void *));
int hashmap_size(Hashmap *map);
void hashmap_free(Hashmap *map, void (*free_value)(void *));

#endif
