#ifndef LIST_H
#define LIST_H

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct {
  Node *head;
  int size;
} List;

List *list_create(void);
void list_push_front(List *list, void *data);
void list_push_back(List *list, void *data);
void *list_pop_front(List *list);
void *list_get(List *list, int index);
int list_size(List *list);
void list_free(List *list, void (*free_item)(void *));

#endif
