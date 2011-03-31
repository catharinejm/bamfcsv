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

}

struct s_Cell *alloc_cell() {

  struct s_Cell *new_cell = malloc(sizeof(struct s_Cell));

  new_cell -> start = 0;
  new_cell -> len = 0;
  new_cell -> next_cell = 0;

}

void free_cell(struct s_Cell *cell) {
  free_cell(cell->next_cell);
  free(cell);
}

void free_row(struct s_Row *row) {
  free_row(row->next_row);
  free_cell(row->first_cell);
  free(row);
}

VALUE build_matrix(char *buf, int bufsize) {

  int str_start = 0;
  int num_rows = 1;

  struct s_Row *first_row = alloc_row();
  struct s_Row *cur_row = first_row;
  struct s_Cell *cur_cell = alloc_cell();
  first_row->first_cell = cur_cell;
  cur_cell->start = buf;

  VALUE matrix;
  VALUE row;
  VALUE new_string;
  int i,j;

  for (i = 0; i < bufsize; i++) {

    if (buf[i] == ',') {

        cur_cell->len = buf+i-(cur_cell->start);
        cur_cell->next_cell = alloc_cell();
        cur_cell = cur_cell->next_cell;
        cur_cell->start = buf+i+1;
        cur_row->cell_count += 1;

    }

    if (buf[i] == '\n') {

      if (buf[i-1] == '\r') 
        cur_cell->len = buf+i-(cur_cell->start-1);
      else
        cur_cell->len = buf+i-(cur_cell->start);

      cur_row->cell_count += 1;
      cur_row->next_row = alloc_row();
      cur_row->first_cell = alloc_cell();
      cur_cell = cur_row->first_cell;
      cur_cell->start = buf+i+1;
      num_rows++;

    }
  }
  
  if (cur_row->cell_count == 0) {
    num_rows--;
  }

  cur_row = first_row;
  matrix = rb_ary_new2(num_rows);

  for (i = 0; i < num_rows; i++) {
    cur_cell = cur_row->first_cell;
    row = rb_ary_new2(cur_row->cell_count);
    rb_ary_store(matrix,i,row);
    for (j = 0; j < cur_row->cell_count; j++) {
      new_string = rb_str_new(cur_cell->start, cur_cell->len);
      rb_ary_store(row, j, new_string);
      cur_cell = cur_cell->next_cell;
    }
    cur_row = cur_row->next_row;
  }

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
