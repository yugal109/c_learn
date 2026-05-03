#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *entries[MAX_HISTORY];
static int count = 0;

void history_add(const char *line) {
  if (strlen(line) == 0) {
    return;
  }

  if (count < MAX_HISTORY) {
    entries[count] = strdup(line);
    count++;
  } else {
    free(entries[0]);
    memmove(entries, entries + 1, (MAX_HISTORY - 1) * sizeof(char *));
    entries[MAX_HISTORY - 1] = strdup(line);
  }
}

void history_print(void) {
  int i;
  for (i = 0; i < count; i++) {
    printf("%d %s\n", i + 1, entries[i]);
  }
}
