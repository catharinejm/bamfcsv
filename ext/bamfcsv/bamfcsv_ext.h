#ifndef _BAMFCSV_EXT_H
#define _BAMFCSV_EXT_H

#include <ruby/ruby.h>

VALUE BAMFCSV_module;
VALUE BAMFCSV_MalformedCSVError_class;

struct bamfcsv_Row {
  struct bamfcsv_Cell *first_cell;
  struct bamfcsv_Row *next_row;
  unsigned long cell_count;
};

struct bamfcsv_Cell {
  char *start;
  int len;
  int has_quotes;
  struct bamfcsv_Cell *next_cell;
};

void Init_bamfcsv();

#endif
