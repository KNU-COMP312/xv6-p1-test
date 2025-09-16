#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int p = fork();
  if(p < 0){
    printf("fork failed\n");
    exit(1);
  }
  if(p == 0){
    // child
    int ppid = getppid();
    printf("child: getppid()=%d\n", ppid);
    exit(0);
  } else {
    // parent
    int my = getpid();
    int st;
    wait(&st);
    printf("parent: my pid=%d (child exited with %d)\n", my, st);
    exit(0);
  }
}
