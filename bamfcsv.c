#include <stdlib.h>
#include <ruby/ruby.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "bamfcsv.h"

#define true 1
#define false 0
typedef unsigned char bool;

int count_newlines(char *buf, int bufsize) {
  int newlines = 0;
  int i;
  for(i = 0; i < bufsize; i++) {
    if (buf[i] == '\n') {
      newlines++;
    }
  }
  return newlines;
}

int *count_cells(char *buf, int bufsize, int lines) {
  int cur_line_count = 1;
  int cur_line = 0;
  int i = 0;
  int *cell_map;
  cell_map = calloc(lines, sizeof(int));
  for (i = 0; i < bufsize; i++) {
    if (buf[i] == ',') {
      cur_line_count++;
    }
    if (buf[i] == '\n') {
      cell_map[cur_line] = cur_line_count;
      cur_line++;
      cur_line_count = 1;
    }
  }
  return cell_map;        
}

VALUE build_matrix(char *buf, int bufsize, int lines, int *cell_map) {
  int str_start = 0;
  int cur_line = 0;
  int cur_cell = 0;
  VALUE matrix = rb_ary_new2(lines);
  VALUE row = rb_ary_new2(cell_map[0]);
  int i;
  for (i = 0; i< lines; i++) {
    if (buf[i] == ',') {
      rb_ary_store(row,cur_cell,rb_str_new(buf+str_start, i-str_start));
      cur_cell++;
      str_start = i+1;
    }
    if (buf[i] = '\n') {
      rb_ary_store(matrix, cur_line, row);
      cur_cell = 0;
      cur_line++;
    }
  }
  return matrix;
}

VALUE mm_parse(const char *file) {
  int csv;
  char *mmapped_csv;
  int newlines;
  int filesize;
  int *cell_counts;
  int i;
  csv = open(file, O_RDONLY);
  filesize = lseek(csv, 0, SEEK_END);
  mmapped_csv = (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, csv, 0);
  newlines = count_newlines(mmapped_csv,filesize);
  cell_counts = count_cells(mmapped_csv,filesize,newlines);
  for (i = 0; i < newlines; i++) {
    printf("line %d cells: %d", i, cell_counts[i]);
  }
  VALUE matrix = build_matrix(mmapped_csv,filesize,newlines,cell_counts);
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
