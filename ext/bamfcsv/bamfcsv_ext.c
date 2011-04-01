#include <stdlib.h>
#include <ruby/ruby.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "bamfcsv_ext.h"

struct s_Row *alloc_row() {

  struct s_Row *new_row = malloc(sizeof(struct s_Row));

  new_row -> first_cell = 0;
  new_row -> next_row = 0;
  new_row -> cell_count = 0;

  return new_row;

}

struct s_Cell *alloc_cell() {

  struct s_Cell *new_cell = malloc(sizeof(struct s_Cell));

  new_cell -> start = 0;
  new_cell -> len = 0;
  new_cell -> next_cell = 0;
  new_cell -> has_quotes = 0;

  return new_cell;

}

void free_cell(struct s_Cell *cell) {

  if (cell != 0) {
    free_cell(cell->next_cell);
    free(cell);
  }

}

void free_row(struct s_Row *row) {

  if (row != 0) {

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
    for (j = 0; j < cur_row->cell_count; j++) {
      if (*(cur_cell->start) == '"' 
          && *((cur_cell->start)+((cur_cell->len-1)*sizeof(char))) == '"')
        new_string = rb_str_new(cur_cell->start+sizeof(char), cur_cell->len-(sizeof(char)*2));
      else
        new_string = rb_str_new(cur_cell->start, cur_cell->len);
      if (cur_cell->has_quotes) {
        rb_funcall(new_string, gsub, 2, dquote, quote);
      }
      rb_ary_store(row, j, new_string);
      cur_cell = cur_cell->next_cell;
    }
    cur_row = cur_row->next_row;
  }

  return matrix;
}

void finalize_cell(struct s_Cell *cell, char *cur) {
  if (*(cur-sizeof(char)) == '\r') 
    cell->len = cur-(cell->start)-sizeof(char);
  else
    cell->len = cur-(cell->start);
}

VALUE build_matrix(char *buf, int bufsize) {
  int str_start = 0;
  int num_rows = 1;
  int in_quote = 0;

  struct s_Row *first_row = alloc_row();
  struct s_Row *cur_row = first_row;
  struct s_Cell *cur_cell = alloc_cell();
  first_row->first_cell = cur_cell;
  cur_cell->start = buf;

  VALUE matrix;

  char *cur;
  
  for (cur = buf; cur < buf+bufsize; cur++) {

    if (*cur == '"') {
      if (in_quote)
        if (*(cur+1) != ',')
          cur_cell->has_quotes = 1;
      in_quote = !in_quote;
    }

    if (!in_quote) {

      if (*cur == ',') {
        
        finalize_cell(cur_cell,cur);
        cur_cell->next_cell = alloc_cell();
        cur_cell = cur_cell->next_cell;
        cur_cell->start = cur+sizeof(char);
        cur_row->cell_count += 1;

      }
      
      if (*cur == '\n') {
        
        finalize_cell(cur_cell,cur);
        cur_row->cell_count += 1;
        cur_row->next_row = alloc_row();
        cur_row = cur_row -> next_row;
        cur_row->first_cell = alloc_cell();
        cur_cell = cur_row->first_cell;
        cur_cell->start = cur+sizeof(char);
        
        num_rows++;

      }
      
    }

  }

  if (cur_row->cell_count == 0) {
    num_rows--;
  }

  matrix = build_matrix_from_pointer_tree(first_row, num_rows);

  free_row(first_row);

  return matrix;

}

VALUE mm_parse(const char *file) {

  char *mmapped_csv;
  int filesize, csv;

  csv = open(file, O_RDONLY);
  filesize = lseek(csv, 0, SEEK_END);
  mmapped_csv = (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, csv, 0);

  VALUE matrix = build_matrix(mmapped_csv,filesize);

  munmap(mmapped_csv, filesize);
  close(csv);

  return matrix;
}

VALUE read_path(VALUE self, VALUE file) {

  return mm_parse(RSTRING_PTR(file));

}

void Init_bamfcsv() {

  VALUE module = rb_define_module("BAMFCSV");
  rb_define_module_function(module, "read_path", read_path, 1);

}
