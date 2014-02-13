//My code
struct ioinfo
{
  FILE* file;
  int numlines;
  char *buf;
  int pid;
};

struct stackheap
{
  pid_t pid;
  struct statusStuff statusstuff;
};

void *stackroutine(void *v)
{
  struct stackheap *stuff = (struct stackheap*)v;
  struct statusStuff s = stuff->statusstuff;
  while(1 && highstack == 0){
  readStatus(stuff->pid, &s);
  if(s.VmStk > 4000)
    {
      highstack++;
      kill(stuff->pid, 9);
      fprintf(stderr, "Stack memory is above 4 MB\n");
    }
 }
}

void *heaproutine(void *v)
{
  struct stackheap *stuff = (struct stackheap*)v;
  struct statusStuff s = stuff->statusstuff;
  while(1 && highheap == 0){
  readStatus(stuff->pid, &s);
  if(s.VmData > 4000)
    {
      highheap++;
      kill(stuff->pid, 9);
      fprintf(stderr, "Heap memory is above 4 MB\n");
    }
  }
}

void *ioroutine(void *v) {
  struct ioinfo *iostuff = (struct ioinfo*)v;
  while(!feof(iostuff->file) && iostuff->numlines <= 100){
 if(iostuff->numlines >= 100)
      {
        fprintf(stderr, "More than 100 lines in child's stdout\n");
        kill(iostuff->pid, SIGKILL);
        break;
      }
    fgets(iostuff->buf, 512, iostuff->file);
    if(iostuff->buf && iostuff->numlines >= 1){
      fprintf(stderr, "child says: %s\n", iostuff->buf);}
    iostuff->numlines++;
  }
}

int main(int argc, char * argv[])
{
  pthread_t thread1, thread2, thread3;
  struct ioinfo iostuff;
  struct stackheap sstuff;
  struct stackheap hstuff;
  char buf[512];
  struct statusStuff s;
 //struct rlimit rlimproc = {3, 3};
  char * executable = malloc(sizeof(*argv[1]));
  strcpy(executable, (const char*)argv[1]);
  //setrlimit(RLIMIT_NPROC, &rlimproc);
  pid_t pid, pid2, w;
  char * argv2[] = {argv[1], NULL};
  char *env[] = {"LD_PRELOAD=./fork.so", NULL};
  int fd[2];
  struct rusage usage;
  pipe(fd);
  pid = fork();
  if(pid == 0)
    {
      close(fd[0]);
      close(fileno(stdout));
      dup(fd[1]);
      execve(executable, argv2, env);
      perror("execl");
      exit(1);
    }
  else if(pid < 0)
 {
      perror("fork");
      exit(1);
    }
  else
    {
      close(fd[1]);
      FILE *read = fdopen(fd[0], "r");
      int numlines = 0;
      iostuff.file = read;
      iostuff.numlines = numlines;
      iostuff.buf = buf;
      iostuff.pid = pid;
      pthread_create(&thread1, NULL, ioroutine, (void *)&iostuff);
      sstuff.statusstuff = s;
      sstuff.pid = pid;
      hstuff.statusstuff = s;
      hstuff.pid = pid;
      pthread_create(&thread2, NULL, stackroutine, (void *)&sstuff);
      pthread_create(&thread3, NULL, heaproutine, (void *)&hstuff);
      w = waitpid(pid, &status, 0);
 if(w == -1)
        {
          perror("waitpid");
          exit(EXIT_FAILURE);
        }
      if(WIFEXITED(status))
        {
          getrusage(RUSAGE_SELF, &usage);
          double u = (double)usage.ru_utime.tv_sec +
            ((double)usage.ru_utime.tv_usec)/1e6;
          double s = (double)usage.ru_stime.tv_sec +
            ((double)usage.ru_stime.tv_usec)/1e6;
          fprintf(stderr, "Total run time is %e\n", u + s);
          time_t clock = time(NULL);
          fprintf(stderr, "Time of death is %s", ctime(&clock));
          fprintf(stderr, "Exit status is %d\n", WEXITSTATUS(status));
          fprintf(stderr, "Lines printed to stdout: %d\n", iostuff.numlines);
        }
      if(WIFSIGNALED(status))
        {
          getrusage(RUSAGE_SELF, &usage);
 double u = (double)usage.ru_utime.tv_sec +
            ((double)usage.ru_utime.tv_usec)/1e6;
          double s = (double)usage.ru_stime.tv_sec +
            ((double)usage.ru_stime.tv_usec)/1e6;
          fprintf(stderr, "Total run time is %e\n", u + s);
          time_t clock = time(NULL);
          fprintf(stderr, "Time of death is %s\n", ctime(&clock));
          fprintf(stderr, "Exit signal is %d\n", WTERMSIG(status));
          fprintf(stderr, "Lines printed to stdout: %d\n", iostuff.numlines);
        }
      exit(0);
    }
}
