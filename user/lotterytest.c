#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 3

struct report {
  int  tickets;
  uint loops;
};

int
main(void)
{
  int tickets[NCHILD] = {10, 20, 40};
  const int window_ticks = 300;
  int ready[2], go[2], rpt[2];

  if (pipe(ready) < 0 || pipe(go) < 0 || pipe(rpt) < 0) {
    printf("pipe failed\n");
    exit(1);
  }

  for (int i = 0; i < NCHILD; i++) {
    int pid = fork();
    if (pid < 0) { printf("fork failed\n"); exit(1); }
    if (pid == 0) {
      close(ready[0]);
      close(go[1]);
      close(rpt[0]);

      if (settickets(tickets[i]) < 0) {
        printf("settickets failed\n");
        exit(1);
      }

      char R = 'R';
      write(ready[1], &R, 1);

      char G;
      if (read(go[0], &G, 1) != 1) exit(1);

      volatile uint acc = 0;
      uint loops = 0;
      int start = uptime();
      int end = start + window_ticks;

      while (uptime() < end) {
        for (int j = 0; j < 1000; j++) {
          acc ^= (uint)j + (acc << 1);
        }
        loops++;
      }

      struct report rep = { tickets[i], loops };
      if (write(rpt[1], (char*)&rep, sizeof(rep)) != sizeof(rep)) exit(1);

      close(ready[1]);
      close(go[0]);
      close(rpt[1]);
      exit(0);
    }
  }

  close(ready[1]);
  close(go[0]);
  close(rpt[1]);

  for (int k = 0; k < NCHILD; k++) {
    char d;
    if (read(ready[0], &d, 1) != 1) { printf("ready read failed\n"); exit(1); }
  }

  char G = 'G';
  write(go[1], &G, 1);
  write(go[1], &G, 1);
  write(go[1], &G, 1);

  for (int k = 0; k < NCHILD; k++) {
    struct report rep;
    if (read(rpt[0], (char*)&rep, sizeof(rep)) != sizeof(rep)) {
      printf("rpt read failed\n");
      exit(1);
    }
    printf("T=%d loops=%u\n", rep.tickets, rep.loops);
  }

  close(ready[0]);
  close(go[1]);
  close(rpt[0]);

  for (int i = 0; i < NCHILD; i++) wait(0);
  exit(0);
}
