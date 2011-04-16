#include <string.h>
#include <stdio.h>

int main() {
  char *s1 = "fo\"obar";
  char *s2 = s1+2;
  char *s3 = s2+1;

  memmove(s2, s3, 5);
  printf("%s\n", s2);
  return 0;
}
