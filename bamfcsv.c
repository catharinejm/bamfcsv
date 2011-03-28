#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bamfcsv.h"

struct s_Parsed *parse(const char *file) {
  FILE *csv = fopen(file, "r"); 

  int bufsize = 256;
  char *buf = calloc(bufsize, sizeof(char));

  int rowcount = 1; // This is absurdly low just to test; make it bigger!
  struct s_Row *rows = calloc(rowcount, sizeof(struct s_Row));
  int line = 0;

  // Assume all lines <= 255 chars for now...
  while (fgets(buf, bufsize, csv)) {
    if (line == rowcount) {
      rowcount *= 2;
      rows = realloc(rows, rowcount*sizeof(struct s_Row));
    }

    rows[line].contents = calloc(strlen(buf)+1, sizeof(char));
    strcpy(rows[line].contents, buf);

    char **cell_p = rows[line].cells;
    int count = 0;
    while ((*cell_p = strsep(&rows[line].contents, ",")) != NULL) {
      ++cell_p;
      ++count;
    }

    rows[line].count = count;

    ++line; 
  }
  struct s_Parsed *parsed_file = malloc(sizeof(struct s_Parsed));
  parsed_file->rows = rows;
  parsed_file->count = line;
  return parsed_file;
}
