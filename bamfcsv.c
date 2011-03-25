#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct s_Row {
  char *contents;
  int count;
  char *cells[10];
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <csv file>\n", argv[0]);
    return 1;
  }

  FILE *csv = fopen(argv[1], "r"); 

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

    /* int cellcount = 10; */
    /* rows[line].cells = calloc(cellcount, sizeof(struct s_Cell)); */

    rows[line].contents = calloc(strlen(buf)+1, sizeof(char));
    strcpy(rows[line].contents, buf);

    char **cell_p = rows[line].cells;
    while ((*cell_p = strsep(&rows[line].contents, ",")) != NULL) {
      ++cell_p;
    }

    ++line; 
  }
  
  printf("Number of lines: %d\n", line);
  int i, j;
  for (i = 0; i < line; ++i) {
    printf("Line %d: ", i+1);
    for(j = 0; j < 10; ++j)
      printf("%s ", rows[i].cells[j]);
  }

  return 0;
}
