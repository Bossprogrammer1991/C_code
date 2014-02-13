#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

struct list {
  struct list *next;
  int sig;
  int count;
} ;

struct timedata {
  double utime;
  double stime;
};

struct list *head=NULL;
struct timedata data;
int alarmnum = 0;
char * lines[10];

void chandler(int sig) {
  printf("Total user time: %e\n", data.utime);
  printf("Total system time: %e\n\n", data.stime);
}

void term_handler(int sig) {
  printf("Total user time: %e\n", data.utime);
  printf("Total system time: %e\n\n", data.stime);
  exit(0);
}

void kill_handler(int sig) {
  int i;
  printf("Last 10 lines of user input:\n");
  for(i=0; i<10; i++)
    {
 if(lines[i] != "NULL")
        {
          printf("%s\n", lines[i]);
        }
    }
  printf("\n");
}

void alrm_handler(int sig) {
  alarmnum += 10;
  printf("tick: %d\n", alarmnum);
}

char *get_line(char *s, int n, FILE *f)
{
  char *p = fgets(s, n, f);
  if(p != NULL) strtok (s, "\n");
  return p;
  if(strlen(p) > 5000) return NULL;
}
main()
{
  int i;
  for(i=0; i<10; i++){lines[i] = "NULL";}
  double u_time_final = 0;
  double s_time_final = 0;
  int alrmnum = 10;
  struct sigaction act, killact, oldact, alrmact, termact;
  // zero values are ignored
  // if I don't do this, everything I don't set
  // has a random value. I don't want that!
  memset(&act, 0, sizeof(struct sigaction));
  memset(&killact, 0, sizeof(struct sigaction));
  memset(&alrmact, 0, sizeof(struct sigaction));
  memset(&termact, 0, sizeof(struct sigaction));
  act.sa_handler = chandler;
  killact.sa_handler = kill_handler;
  alrmact.sa_handler = alrm_handler;
  termact.sa_handler = term_handler;
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGINT);
   sigaddset(&act.sa_mask, SIGQUIT);
  sigemptyset(&killact.sa_mask);
  sigaddset(&killact.sa_mask, SIGTSTP);
  sigemptyset(&alrmact.sa_mask); //
  sigaddset(&alrmact.sa_mask, SIGALRM);
  sigemptyset(&termact.sa_mask);
  sigaddset(&termact.sa_mask, SIGTERM);
  act.sa_flags = 0; // no special flags
  sigaction(SIGQUIT, &act, &oldact);
  sigaction(SIGINT, &act, &oldact);
  sigaction(SIGTSTP, &killact, &oldact);
  sigaction(SIGALRM, &alrmact, &oldact);
  sigaction(SIGTERM, &termact, &oldact);
  while(1) {
    struct rusage usage;
    char * buf = NULL;
    buf = (char*)realloc(buf, 5000* sizeof(char));
    int foundnull = 0;
    alarm(10);
    char *line = get_line(buf, 5000, stdin);
    for(i=0; i<10; i++)
 {
        if((lines[i] == "NULL") && line && foundnull == 0)
          {
            lines[i] = line;
            foundnull = 1;
          }
        else if((lines[i] == "NULL" && foundnull == 1))
          {
            break;
          }
      }
    if(!foundnull && line)
      {
        for(i=0; i<9; i++)
          {
            lines[i] = lines[i+1];
          }
        lines[9] = line;
      }
    sleep(1);
    getrusage(RUSAGE_SELF, &usage);
double u_time = (double)usage.ru_utime.tv_sec +
      ((double)usage.ru_utime.tv_usec)/1e6;
    double s_time = (double)usage.ru_stime.tv_sec +
      ((double)usage.ru_stime.tv_usec)/1e6;
    u_time_final += u_time;
    s_time_final += s_time;
    data.utime = u_time_final;
    data.stime = s_time_final;
  }
}
