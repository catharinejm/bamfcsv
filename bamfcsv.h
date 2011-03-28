#ifndef _BAMFCSV_H
#define _BAMFCSV_H

struct s_Row {
  char *contents;
  int count;
  char *cells[10];
};

struct s_Row *parse(const char*, int*);

#endif
