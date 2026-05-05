#include "list.h"
#include <stdio.h>
#include <stdlib.h>

List *list_create(void)
{
  List *list = malloc(sizeof(List));
  if (list == NULL)
  {
    fprintf(stderr, "list: memeory allocation failed.\n");
    exit(1);
  }
  list->head = NULL;
  list->size = 0;
  return list;
}

void list_push_front(List *list, void *data)
{
  Node *node = malloc(sizeof(Node));
  if (node == NULL)
  {
    fprintf(stderr, "list: memory allocation failed.\n");
    exit(1);
  }

  node->data = data;
  node->next = list->head;
  list->head = node;
  list->size++;
}

void list_push_back(List *list, void *data)
{
  Node *node = malloc(sizeof(Node));
  if (node == NULL)
  {
    fprintf(stderr, "list: memory allocation failed.\n");
    exit(1);
  }
  node->data = data;
  node->next = NULL;
  if (list->head == NULL)
  {
    list->head = node;
  }
  else
  {
    Node *current = list->head;
    while (current->next != NULL)
    {
      current = current->next;
    }
    current->next = node;
  }
  list->size++;
}

void *list_pop_front(List *list)
{
  if (list->head == NULL)
  {
    return NULL;
  }
  Node *node = list->head;
  void *data = node->data;
  list->head = node->next;
  free(node);
  list->size--;
  return data;
}

void *list_get(List *list, int index)
{
  if (index < 0 || index >= list->size)
  {
    fprintf(stderr, "list: index %d out of bounds\n", index);
    return NULL;
  }

  Node *current = list->head;
  int i;
  for (i = 0; i < index; i++)
    current = current->next;

  return current->data;
}

int list_size(List *list) { return list->size; }

void list_free(List *list, void (*free_item)(void *))
{
  Node *current = list->head;
  while (current != NULL)
  {
    Node *next = current->next;
    if (free_item != NULL)
    {
      free_item(current->data);
    }
    free(current);
    current = next;
  }
  free(list);
}
