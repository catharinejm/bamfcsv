#include <ruby/ruby.h>
#include <ruby/encoding.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

VALUE BAMFCSV_module;
VALUE BAMFCSV_MalformedCSVError_class;

VALUE bamfcsv_finalize_cell(char *cell_start, char *cell_end, int quote_count, rb_encoding *enc) {
  if (*cell_end == '\r')
    cell_end--;

  if (cell_end < cell_start)
    return Qnil;

  if (quote_count) {
    cell_start++;
    cell_end--;
  }

  VALUE cell_str = rb_enc_str_new(cell_start, cell_end-cell_start+1, enc);

  return cell_str;
}

bool quotes_end_line(char* cur) {
  return *(cur-1) == '"' || (*(cur-1) == '\r' && *(cur-2) == '"');
}

VALUE bamfcsv_parse_string(VALUE self, VALUE string, VALUE rstr_sep) {
  char *buf = RSTRING_PTR(string);
  long bufsize = RSTRING_LEN(string);
  rb_encoding *enc = rb_enc_from_index(ENCODING_GET(string));
  char separator = *RSTRING_PTR(rstr_sep);

  unsigned long num_rows = 1, cell_count = 1;
  int quote_count = 0, quotes_matched = 1;

  VALUE matrix = rb_ary_new();
  VALUE row = rb_ary_new();

  char *cur = buf, *cell_start = buf;

  if (bufsize > 0 && *(buf+bufsize-1) == '\n') {
    *(buf+bufsize-1) = 0;
    --bufsize;
  }

  VALUE dbl_dquote = rb_str_new("\"\"", 2), dquote = rb_str_new("\"", 1);
  ID gsub_bang = rb_intern("gsub!");
  
  for (; cur < buf+bufsize; cur++) {

    if (*cur == '"') {
      if (0 == quote_count && cell_start != cur) /* Quotes begin past opening of cell */
        rb_raise(BAMFCSV_MalformedCSVError_class, "Illegal quoting on line %lu, cell %lu: Quoted cell must open with '\"'", num_rows, cell_count);
      else
        ++quote_count;
    }

    quotes_matched = !(quote_count & 1); /* count is even */

    if (quotes_matched) { 

      if (*cur == separator) {
        
        if (quote_count && *(cur-1) != '"')
          rb_raise(BAMFCSV_MalformedCSVError_class, "Unclosed quoted field on line %lu, cell %lu.", num_rows, cell_count);

        VALUE cell_str = bamfcsv_finalize_cell(cell_start, cur-1, quote_count, enc);
        if (quote_count)
          rb_funcall(cell_str, gsub_bang, 2, dbl_dquote, dquote);

        rb_ary_push(row, cell_str);
        cell_start = cur+1;

        quote_count = 0;
        ++cell_count;

      } else if (*cur == '\n') {
        
        if (quote_count && !quotes_end_line(cur))
            rb_raise(BAMFCSV_MalformedCSVError_class, "Unclosed quoted field on line %lu, cell %lu: EOL", num_rows, cell_count);

        VALUE cell_str = bamfcsv_finalize_cell(cell_start, cur-1, quote_count, enc);
        if (quote_count)
          rb_funcall(cell_str, gsub_bang, 2, dbl_dquote, dquote);
        /* Completely blank lines don't even get a nil. This matches CSV's behavior. */
        if (cell_count > 1 || cell_str != Qnil)
          rb_ary_push(row, cell_str);
        rb_ary_push(matrix, row);
        row = rb_ary_new();
        cell_start = cur+1;
        
        quote_count = 0;
        ++num_rows;
        cell_count = 0;

      } else if (quote_count && *cur != '\r' && *cur != '"')
        rb_raise(BAMFCSV_MalformedCSVError_class, "Illegal quoting on line %lu, cell %lu", num_rows, cell_count);
      
    }

  }

  if (!quotes_matched) /* Reached EOF without matching quotes */
    rb_raise(BAMFCSV_MalformedCSVError_class, "Illegal quoting on line %lu, cell %lu: File ends without closing '\"'", num_rows, cell_count);
  else if (quote_count && !quotes_end_line(cur)) /* Quotes closed before end of final cell */
    rb_raise(BAMFCSV_MalformedCSVError_class, "Unclosed quoted field on line %lu, cell %lu: EOF", num_rows, cell_count);

  VALUE cell_str = bamfcsv_finalize_cell(cell_start, cur-1, quote_count, enc);
  if (quote_count)
    rb_funcall(cell_str, gsub_bang, 2, dbl_dquote, dquote);
  /* Completely blank lines don't even get a nil. This matches CSV's behavior. */
  if (cell_count > 1 || cell_str != Qnil)
    rb_ary_push(row, cell_str);
  rb_ary_push(matrix, row);

  return matrix;

}

void Init_bamfcsv() {

  BAMFCSV_module = rb_define_module("BAMFCSV");
  VALUE bamfcsv_singleton_class = rb_singleton_class(BAMFCSV_module);
  rb_define_private_method(bamfcsv_singleton_class, "__parse_string", bamfcsv_parse_string, 2);

  BAMFCSV_MalformedCSVError_class = rb_define_class_under(BAMFCSV_module, "MalformedCSVError", rb_eRuntimeError);
}
