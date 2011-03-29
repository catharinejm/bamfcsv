#ifndef _BAMFCSV_H
#define _BAMFCSV_H

#define BAMF_DFLT_ROWC 32
#define BAMF_DFLT_CELLC 4
#define BAMF_DFLT_CLEN 64

struct s_Row {
  char **cells;
  int count;
};

struct s_Parsed {
  struct s_Row *rows;
  int count;
};

struct s_Parsed *parse(const char*);

#endif
