#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int numprocs = 0;
pid_t fork(void)
{
  static pid_t (*fork1)(void) = NULL;
  pid_t child;
  if(!fork1)
    {
      fork1 = dlsym(RTLD_NEXT, "fork");
    }
  if(numprocs >= 2)
    {
      fprintf(stderr, "Child forked too many processes\n");
      kill(0, SIGKILL);
      child = 0;
    }
  else
    {
      numprocs++;
      child = fork1();
    }
  return child;
}

FILE *fopen(const char *path, const char *mode)
{
  fprintf(stderr, "Child opened illegal file.\n");
 kill(0, SIGKILL);
  return NULL;
}

