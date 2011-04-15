#include <stdlib.h>
#include "bamfcsv_ext.h"

struct s_Cell *alloc_cell(struct s_Row *row, struct s_Cell *prev_cell) {

  struct s_Cell *new_cell = malloc(sizeof(struct s_Cell));

  new_cell -> start = NULL;
  new_cell -> len = 0;
  new_cell -> next_cell = NULL;
  new_cell -> has_quotes = 0;
  row->cell_count++;
  if (prev_cell != NULL) prev_cell->next_cell = new_cell;

  return new_cell;

}

struct s_Row *alloc_row(struct s_Row *prev_row) {

  struct s_Row *new_row = malloc(sizeof(struct s_Row));

  new_row -> next_row = NULL;
  new_row -> cell_count = 0;
  new_row -> first_cell = alloc_cell(new_row, NULL);
  if (prev_row != NULL) prev_row->next_row = new_row;

  return new_row;

}

void free_cell(struct s_Cell *cell) {

  if (cell != NULL) {
    free_cell(cell->next_cell);
    free(cell);
  }

}

void free_row(struct s_Row *row) {

  if (row != NULL) {

    free_row(row->next_row);
    free_cell(row->first_cell);
    free(row);

  }

}

VALUE build_matrix_from_pointer_tree(struct s_Row *first_row, int num_rows) {
  VALUE matrix;
  VALUE row;
  VALUE new_string;
  int i,j;
  struct s_Row *cur_row;
  struct s_Cell *cur_cell;

  cur_row = first_row;
  matrix = rb_ary_new2(num_rows);

  ID gsub = rb_intern("gsub!");
  VALUE dquote = rb_str_new2("\"\""), quote = rb_str_new2("\"");

  for (i = 0; i < num_rows; i++) {

    cur_cell = cur_row->first_cell;
    row = rb_ary_new2(cur_row->cell_count);
    rb_ary_store(matrix,i,row);
    if (cur_row->cell_count > 1 || cur_cell->len) {
      for (j = 0; j < cur_row->cell_count; j++) {
        if (cur_cell->has_quotes) {
          new_string = rb_str_new(cur_cell->start+1, cur_cell->len-2);
          rb_funcall(new_string, gsub, 2, dquote, quote);
        } else {
          if (cur_cell->len)
            new_string = rb_str_new(cur_cell->start, cur_cell->len);
          else
            new_string = Qnil; /* Empty, unquoted cells are nil, for default ruby CSV compatibility */
        }
        rb_ary_store(row, j, new_string);
        cur_cell = cur_cell->next_cell;
      }
    }
    cur_row = cur_row->next_row;
  }

  return matrix;
}

void finalize_cell(struct s_Cell *cell, char *cur, int quote_count) {
  if (*(cur-1) == '\r') 
    cell->len = (int)(cur-(cell->start)-1);
  else
    cell->len = (int)(cur-(cell->start));

  if (quote_count) cell->has_quotes = 1;
}

VALUE build_matrix(char *buf, int bufsize) {
  int str_start = 0;
  int num_rows = 1;
  int quote_count = 0, quotes_matched = 1;

  struct s_Row *first_row = alloc_row(NULL);
  struct s_Row *cur_row = first_row;
  struct s_Cell *cur_cell = cur_row->first_cell;
  cur_cell->start = buf;

  VALUE matrix;

  char *cur;

  if (bufsize > 0 && *(buf+bufsize-1) == '\n') {
    *(buf+bufsize-1) = 0;
    --bufsize;
  }
  
  for (cur = buf; cur < buf+bufsize; cur++) {

    if (*cur == '"') {
      if (0 == quote_count && cur_cell->start != cur) /* Quotes begin past opening of cell */
        rb_raise(BAMFCSV_MalformedCSVError_class, "Illegal quoting on line %d, cell %d: Quoted cell must open with '\"'", num_rows, cur_row->cell_count);
      else
        ++quote_count;
    }

    quotes_matched = !(quote_count & 1); /* count is even */

    if (quotes_matched) { 

      if (*cur == ',') {
        
        if (quote_count && *(cur-1) != '"')
          rb_raise(BAMFCSV_MalformedCSVError_class, "Unclosed quoted field on line %d, cell %d.", num_rows, cur_row->cell_count);

        finalize_cell(cur_cell, cur, quote_count);
        cur_cell = alloc_cell(cur_row, cur_cell);
        cur_cell->start = cur+1;
        quote_count = 0;

      } else if (*cur == '\n') {
        
        if (quote_count && !(*(cur-1) == '"' || *(cur-1) == '\r' && *(cur-2) == '"'))
            rb_raise(BAMFCSV_MalformedCSVError_class, "Unclosed quoted field on line %d, cell %d: EOL", num_rows, cur_row->cell_count);

        finalize_cell(cur_cell, cur, quote_count);
        cur_row = alloc_row(cur_row);
        cur_cell = cur_row->first_cell;
        cur_cell->start = cur+1;
        quote_count = 0;
        
        num_rows++;

      } else if (quote_count && *cur != '\r' && *cur != '"')
        rb_raise(BAMFCSV_MalformedCSVError_class, "Illegal quoting on line %d, cell %d", num_rows, cur_row->cell_count);
      
    }

  }

  if (!quotes_matched) /* Reached EOF without matching quotes */
    rb_raise(BAMFCSV_MalformedCSVError_class, "Illegal quoting on line %d, cell %d: File ends without closing '\"'", num_rows, cur_row->cell_count);
  else if (quote_count && *(cur-1) != '"') /* Quotes closed before end of final cell */
    rb_raise(BAMFCSV_MalformedCSVError_class, "Unclosed quoted field on line %d, cell %d: EOF", num_rows, cur_row->cell_count);

  finalize_cell(cur_cell, cur, quote_count);

  matrix = build_matrix_from_pointer_tree(first_row, num_rows);

  free_row(first_row);

  return matrix;

}

VALUE parse_string(VALUE self, VALUE string) {

  return build_matrix(RSTRING_PTR(string), NUM2INT(rb_str_length(string)));

}

void Init_bamfcsv() {

  BAMFCSV_module = rb_define_module("BAMFCSV");
  VALUE bamfcsv_singleton_class = rb_singleton_class(BAMFCSV_module);
  rb_define_private_method(bamfcsv_singleton_class, "__parse_string", parse_string, 1);

  BAMFCSV_MalformedCSVError_class = rb_define_class_under(BAMFCSV_module, "MalformedCSVError", rb_eRuntimeError);
}
