#include <stdlib.h>
#include <ruby/ruby.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "bamfcsv.h"

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

  return new_cell;

}

void free_cell(struct s_Cell *cell) {

  if (cell -> next_cell != 0)
    free_cell(cell->next_cell);
  free(cell);

}

void free_row(struct s_Row *row) {

  if (row->next_row != 0) 
    free_row(row->next_row);
  if (row->first_cell != 0)
    free_cell(row->first_cell);
  free(row);

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
      rb_ary_store(row, j, new_string);
      cur_cell = cur_cell->next_cell;
    }
    cur_row = cur_row->next_row;
  }

  return matrix;
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
        in_quote = 0;
      else
        in_quote = 1;
    }

    if (!in_quote) {

      if (*cur == ',') {
        
        cur_cell->len = cur-(cur_cell->start);
        cur_cell->next_cell = alloc_cell();
        cur_cell = cur_cell->next_cell;
        cur_cell->start = cur+sizeof(char);
        cur_row->cell_count += 1;
        
      }
      
      if (*cur == '\n') {
        
        if (*(cur-sizeof(char)) == '\r') 
          cur_cell->len = cur-(cur_cell->start)-sizeof(char);
        else
          cur_cell->len = cur-(cur_cell->start);
        
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

VALUE do_the_parsing(VALUE self, VALUE file) {

  return mm_parse(RSTRING_PTR(file));

}

void Init_bamfcsv() {

  VALUE module = rb_define_module("BAMFCSV");
  rb_define_module_function(module, "parse", do_the_parsing, 1);

}
