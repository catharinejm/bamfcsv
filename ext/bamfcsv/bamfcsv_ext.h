#ifndef _BAMFCSV_EXT_H
#define _BAMFCSV_EXT_H

struct s_Row {
  struct s_Cell *first_cell;
  struct s_Row *next_row;
  int cell_count;
};

struct s_Cell {
  char *start;
  int len;
  struct s_Cell *next_cell;
};

void Init_bamfcsv();

#endif
