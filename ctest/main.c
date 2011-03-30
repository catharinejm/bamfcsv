#include <stdio.h>
#include "../bamfcsv.h"

int main(int argc, char *argv[]) {
  struct s_Parsed *results = parse(argv[1]);

  int i, j;

  if (argc < 3) {
    for (i = 0; i < results->count; ++i) {
      printf("Line %d: ", i+1);
      for (j = 0; j < results->rows[i].count; ++j) {
        printf("%s", results->rows[i].cells[j]);
        if (j + 1 < results->rows[i].count) printf(" | ");
      }
      printf("\n");
    }
  }
  return 0;
}
