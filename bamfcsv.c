#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bamfcsv.h"

struct s_Parsed *parse(const char *file) {
  FILE *csv = fopen(file, "r"); 

  int bufsize = 256;
  char *buf = calloc(bufsize, sizeof(char));

  int rowcount = 256;
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

    int cellcount = 16;
    rows[line].cells = calloc(cellcount, sizeof(char**));
    char **cell_p = rows[line].cells;
    int count = 0;
    while ((*cell_p = strsep(&rows[line].contents, ",")) != NULL) {
      ++count;
      if (count == cellcount) {
        cellcount *= 2;
        rows[line].cells = realloc(rows[line].cells, cellcount*sizeof(char**));
      }
      cell_p = rows[line].cells + count;
    }

    rows[line].count = count;

    ++line; 
  }
  struct s_Parsed *parsed_file = malloc(sizeof(struct s_Parsed));
  parsed_file->rows = rows;
  parsed_file->count = line;
  return parsed_file;
}
