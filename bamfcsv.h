#ifndef _BAMFCSV_H
#define _BAMFCSV_H

#define BAMF_DFLT_ROWC 32
#define BAMF_DFLT_CELLC 4
#define BAMF_DFLT_CLEN 128

struct s_Row {
  char **cells;
  int count;
};

struct s_Parsed {
  struct s_Row *rows;
  int count;
};

int count_newlines(char *buf, int bufsize);

int *count_cells(char *buf, int bufsize, int lines);

VALUE mm_parse(const char *file);

struct s_Parsed *parse(const char*);

VALUE do_the_parsing(VALUE self, VALUE file);

void Init_bamfcsv();

#endif
