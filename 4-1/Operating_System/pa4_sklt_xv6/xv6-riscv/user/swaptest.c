#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int
main(void)
{
  char *a[40000];
  int i;

  for (i = 0; i < 40000; i++) {
    a[i] = sbrk(4096);
    if (a[i] == (char*)-1) {
      exit(1);
    }
    a[i][0] = (char)i;
  }
  exit(0);
}