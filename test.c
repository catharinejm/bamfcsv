#include <ruby/ruby.h>
#include <stdio.h>
#include "bamfcsv.h"

VALUE do_the_parsing(VALUE self) {
  int BAMFrowcount;
  struct s_Row *BAMFrows = parse("../README", &BAMFrowcount);
  VALUE ary = rb_ary_new();
  int i, j;
  for(i = 0; i < BAMFrowcount; ++i) {
    VALUE row = rb_ary_new();
    for(j = 0; j < 10 && BAMFrows[i].count > 0; ++j) {
      rb_str_new2(BAMFrows[i].cells[j]);
//      rb_ary_push(row, rb_str_new2(BAMFrows[i].cells[j]));
    }
    rb_ary_push(ary, row);
  }
  return rb_iv_set(self, "parsed_file", ary);
}

void Init_test() {
  VALUE module = rb_define_module("ExtensionTest");
  rb_define_const(module, "Value", rb_str_new2("Hello World"));
  rb_define_method(module, "parse", do_the_parsing, 0);
}
