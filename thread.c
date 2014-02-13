#include "/comp/111/a/a4/anthills.h"
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

int initialized=FALSE; // semaphores and mutexes are not initialized

// define your mutexes and semaphores here
// they must be global variables.

pthread_mutex_t mymutex;
pthread_mutex_t sleepmutex;
sem_t nhillone;
sem_t nhilltwo;
sem_t nhillthree;
sem_t nantsone;
sem_t nantstwo;
sem_t nantsthree;

void *sleeper(void *v)
{
  sem_t *s = (sem_t*)v;
  struct timespec swallow_time;
  swallow_time.tv_sec = (time_t) 1;
  swallow_time.tv_nsec = (long)100000000;
  struct timespec rem;
  nanosleep(&swallow_time, &rem);
  sem_post(s);
}

// first thread initializes mutexes
void *aardvark(void *input) {
  pthread_t thread1;
    char aname = *(char *)input;
    // first caller need to initialize the mutexes!
    if (!initialized++) { // this succeeds only for one thread
        // initialize your mutexes and semaphores here
      pthread_mutex_init(&mymutex, NULL);
      pthread_mutex_init(&sleepmutex, NULL);
      sem_init(&nhillone, 0, 3);
 sem_init(&nhilltwo, 0, 3);
      sem_init(&nhillthree, 0, 3);
      sem_init(&nantsone, 0, 100);
      sem_init(&nantstwo, 0, 100);
      sem_init(&nantsthree, 0, 100);
    }

    // now be an aardvark
    while (chow_time()) {
      if(!sem_trywait(&nhillone)){
        if(!sem_trywait(&nantsone)){
          pthread_create(&thread1, NULL, sleeper, (void*)&nhillone);
        slurp(aname,0);}}
      else if(!sem_trywait(&nhilltwo)){
        if(!sem_trywait(&nantstwo)){
          pthread_create(&thread1, NULL, sleeper, (void*)&nhilltwo);
        slurp(aname, 1);}}
      else if(!sem_trywait(&nhillthree)){
        if(!sem_trywait(&nantsthree)){
          pthread_create(&thread1, NULL, sleeper, (void*)&nhillthree);
        slurp(aname, 2);}}
  else{}
    }
    return NULL;
}
