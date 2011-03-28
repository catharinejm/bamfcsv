#include <ruby/ruby.h>
#include <stdio.h>
#include "bamfcsv.h"

VALUE do_the_parsing(VALUE self, VALUE file) {
  struct s_Parsed *parsed_file = parse(RSTRING_PTR(file));
  VALUE ary = rb_ary_new();
  int i, j;
  struct s_Row *cur_row;
  for(i = 0, cur_row = parsed_file->rows; i < parsed_file->count; ++i, ++cur_row) {
    VALUE row = rb_ary_new();
    char **cur_cell;
    for(j = 0, cur_cell = cur_row->cells; j < cur_row->count; ++j, ++cur_cell) {
      rb_ary_push(row, rb_str_new2(*cur_cell));
    }
    rb_ary_push(ary, row);
  }
  return rb_iv_set(self, "@parsed_file", ary);
}

void Init_test() {
  VALUE module = rb_define_module("BAMFCSV");
  rb_define_method(module, "parse", do_the_parsing, 1);
}
