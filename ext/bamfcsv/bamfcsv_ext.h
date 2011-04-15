#ifndef _BAMFCSV_EXT_H
#define _BAMFCSV_EXT_H

#include <ruby/ruby.h>

VALUE BAMFCSV_module;
VALUE BAMFCSV_MalformedCSVError_class;

struct s_Row {
  struct s_Cell cells[];
  struct s_Row *next_row;
  unsigned long cell_count;
};

struct s_Cell {
  char *start;
  int len;
  int has_quotes;
};

void Init_bamfcsv();

#endif
