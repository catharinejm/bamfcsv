#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bamfcsv.h"

struct s_Row* parse(const char *file, int *count) {
  FILE *csv = fopen(file, "r"); 

  int bufsize = 256;
  char *buf = calloc(bufsize, sizeof(char));

  int BAMFrowcount = 1; // This is absurdly low just to test; make it bigger!
  struct s_Row *BAMFrows = calloc(BAMFrowcount, sizeof(struct s_Row));
  int line = 0;

  // Assume all lines <= 255 chars for now...
  while (fgets(buf, bufsize, csv)) {
    if (line == BAMFrowcount) {
      BAMFrowcount *= 2;
      BAMFrows = realloc(BAMFrows, BAMFrowcount*sizeof(struct s_Row));
    }

    BAMFrows[line].contents = calloc(strlen(buf)+1, sizeof(char));
    strcpy(BAMFrows[line].contents, buf);

    char **cell_p = BAMFrows[line].cells;
    int count = 0;
    while ((*cell_p = strsep(&BAMFrows[line].contents, ",")) != NULL) {
      ++cell_p;
      ++count;
    }

    BAMFrows[line].count = count;

    ++line; 
  }
  *count = BAMFrowcount;
  return BAMFrows;
}
