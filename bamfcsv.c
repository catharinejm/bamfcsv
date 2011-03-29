#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bamfcsv.h"

#define true 1
#define false 0
typedef unsigned char bool;

struct s_Parsed *parse(const char *file) {
  FILE *csv = fopen(file, "r"); 

  int num_rows = 32;
  struct s_Row *rows = calloc(num_rows, sizeof(struct s_Row));
  struct s_Row *cur_row = rows;

  int num_cells = 4;
  cur_row->cells = calloc(num_cells, sizeof(char**));
  char **cur_cell = cur_row->cells;

  int cell_length = 64;
  *cur_cell = calloc(cell_length, sizeof(char));
  char *write_p = *cur_cell;

  int rowcount = 0, cellcount = 0;
  bool is_quoted = false, write_quote = false;

  while (!feof(csv) && !ferror(csv)) {
    char cur_char = fgetc(csv), next_char;

    if ('\n' == cur_char) {
      cur_row->count = cellcount + 1;
      ++cur_row; // TODO: Need overflow check (num_rows)

      cur_row->cells = calloc(num_cells, sizeof(char**));
      cur_cell = cur_row->cells;

      *cur_cell = calloc(cell_length, sizeof(char));
      write_p = *cur_cell;
      cellcount = 0;

      ++rowcount;
    } else if (',' == cur_char && !is_quoted) {
      ++cur_cell; // TODO: Need overflow check (num_cells)
      *cur_cell = calloc(cell_length, sizeof(char));
      write_p = *cur_cell;
      ++cellcount;
    } else if ('"' == cur_char && !write_quote) {
      next_char = fgetc(csv);

      if ('"' == next_char)
        write_quote = true;
      else
        is_quoted = !is_quoted;

      ungetc(next_char, csv);
    } else {
      *write_p = cur_char;
      ++write_p; // TODO: Need overflow check (cell_length-1, Needs NULL termination)
      write_quote = false;
    }
  }

  struct s_Parsed *parsed_file = malloc(sizeof(struct s_Parsed));
  parsed_file->rows = rows;
  parsed_file->count = rowcount + 1;
  return parsed_file;
}
