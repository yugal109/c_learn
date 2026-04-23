#include <stdio.h>
#include <string.h>
#include "search.h"

int main(int argc, char *argv[])
{
    int show_lines = 0;
    int i = 1;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: cgrep [-n] pattern file...\n");
        return 1;
    }

    if (strcmp(argv[1], "-n") == 0)
    {
        show_lines = 1;
        i = 2;
    }

    char *pattern = argv[i];
    i++;

    for (; i < argc; i++)
    {
        search_in_file(argv[i], pattern, show_lines);
    }
    return 0;
}
