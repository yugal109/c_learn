#include <stdio.h>
#include <stdlib.h>
#include "input.h"

#define INITIAL_BUFFER_SIZE 64

char *read_line(void)
{
  int bufsize = INITIAL_BUFFER_SIZE;
  int position=0;
  char *buffer=malloc(bufsize);
  int c;

  if (buffer==NULL)
  {
    fprintf(stderr,"yugshell: memory alllocation failed\n");
    exit(1);
  }

  while(1)
  {
    c = getchar();

    if(c == EOF || c == '\n')
    {
      buffer[position]='\0';
      return buffer;
    }

    buffer[position]=c;
    position++;

    if(position>=bufsize)
    {
      bufsize*=2;
      buffer=realloc(buffer,bufsize);
      if(buffer==NULL){
        fprintf(stderr,"yugshell: memory alllocation failed\n");
        exit(1);
      }
    }

  }
}


