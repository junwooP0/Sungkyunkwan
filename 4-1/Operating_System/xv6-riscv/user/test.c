#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "syscall.h"



int
main(void){
int fd = open("README", O_RDONLY);
printf("freemem: %d\n",freemem());
char* readme = (char*)mmap(0, 8192, PROT_READ, MAP_POPULATE, fd, 0);

printf("freemem(after readme): %d\n",freemem());
printf("readme: %s\n", readme);
}
