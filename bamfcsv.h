#ifndef _BAMFCSV_H
#define _BAMFCSV_H

struct s_Row {
  char *contents;
  int count;
  char **cells;
};

struct s_Parsed {
  struct s_Row *rows;
  int count;
};

struct s_Parsed *parse(const char*);

#endif
