// Examples of how to call salient timing functions

#define _POSIX_C_SOURCE 199309
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

// not declared in standard headers
extern void *sbrk(intptr_t);

// read cpu time
inline void get_time(double *user, double *system) {
}

// calculate the cycles of the high resolution clock per accounting clock tick,
// by waiting through 100 ticks and dividing.
inline double seconds_per_tick()
{
    struct timespec res;
    clock_getres(CLOCK_REALTIME, &res);
    double resolution = res.tv_sec + (((double)res.tv_nsec)/1.0e9);
    return resolution;
}

int main() {
    printf("Times mentioned in circumstance statments are as shown in regular linux.cs.tufts.edu.\n\n");
    int success;
    int i;
    struct rusage repeats;
    for(i = 0; i < 100000; i++)
      {
      }
    getrusage(RUSAGE_SELF, &repeats);
    double repeat_u = (double)repeats.ru_utime.tv_sec +
 ((double)repeats.ru_utime.tv_usec)/1e6;
    double repeat_s = (double)repeats.ru_stime.tv_sec +
      ((double)repeats.ru_stime.tv_usec)/1e6;
    struct rusage file_repeats;
    for(i = 0; i < 30000; i++)
      {
      }
    getrusage(RUSAGE_SELF, &file_repeats);
    double file_repeat_u = (double)file_repeats.ru_utime.tv_sec +
      ((double)file_repeats.ru_utime.tv_usec)/1e6;
    double file_repeat_s = (double)file_repeats.ru_stime.tv_sec +
      ((double)file_repeats.ru_stime.tv_usec)/1e6;
    printf("CIRCUMSTANCE: Repeating a function 100000 times takes between 7 and 9 e-03 user time, and between 2 and 3 e-03 system time.\n\n");
    // how to invoke mutex locks
    pthread_mutex_t onelock;
    pthread_mutex_init(&onelock,NULL);
    double mutex_u;
    double mutex_s;
    double mutex_u_final = 0;
    double mutex_s_final = 0;
    int j;
 for(j = 0; j < 100000; j++)
      {
        struct rusage mutexbuf;
        pthread_mutex_lock(&onelock);
        getrusage(RUSAGE_SELF, &mutexbuf);
        mutex_u = (double)mutexbuf.ru_utime.tv_sec +
           ((double)mutexbuf.ru_utime.tv_usec)/1e6;
        mutex_u_final = mutex_u_final + mutex_u;
        mutex_s = (double)mutexbuf.ru_stime.tv_sec +
           ((double)mutexbuf.ru_stime.tv_usec)/1e6;
        mutex_s_final = mutex_s_final + mutex_s;
        pthread_mutex_unlock(&onelock);
      }
    printf("mutex user=%e\n", (mutex_u_final - repeat_u)/100000);
    printf("mutex system=%e\n", (mutex_s_final - repeat_s)/100000);
    printf("CIRCUMSTANCE: Mutex lock call takes far shorter when called the first time before an unlock than any subsequent time after. Original user and system times \
are around 9 e-04 and 3e-03 respectively while average times are in order of magnitude of e-02.\n\n");
    // how to manipulate semaphores
    sem_t onesem;
    double sem_u;
    double sem_s;
    double sem_u_final = 0;
    double sem_s_final = 0;
    success = sem_init(&onesem,0,0);
    if (success<0) perror("sem_init");
    success = sem_post(&onesem);
    if (success<0) perror("sem_post");
    int k;
    for(k = 0; k < 100000; k++)
      {
        struct rusage sembuf;
        sem_post(&onesem);
        getrusage(RUSAGE_SELF, &sembuf);
        sem_u = (double)sembuf.ru_utime.tv_sec +
          ((double)sembuf.ru_utime.tv_usec)/1e6;
        sem_u_final = sem_u_final + sem_u;
        sem_s = (double)sembuf.ru_stime.tv_sec +
          ((double)sembuf.ru_stime.tv_usec)/1e6;
        sem_s_final = sem_s_final + sem_s;
      }
    printf("sem_post user=%e\n", (sem_u_final - repeat_u)/100000);
 printf("sem_post system=%e\n", (sem_s_final - repeat_s)/100000);
    printf("CIRCUMSTANCE: System times with sem_post vary more than user calls, and sem_post times are shorter when first called. User times range from 4.1 to 5.6 e-02\
 while system times range from 1.1 to 1.5 e-01 on average. But single call times for user and system are roughly 3 e-02 and 9 e-02 respectively.\n\n");
    // how to open a file
    double open_u_short = 0;
    double open_s_short = 0;
    double open_u_short_open = 0;
    double open_s_short_open = 0;
    double open_u;
    double open_s;
    int fd;
    for(i=0; i < 10000; i++) //short path not yet open
      {
        struct rusage openbuf;
        fd = open("/etc/motd", O_RDONLY);
        getrusage(RUSAGE_SELF, &openbuf);
        open_u = (double)openbuf.ru_utime.tv_sec +
          ((double)openbuf.ru_utime.tv_usec)/1e6;
        open_s = (double)openbuf.ru_stime.tv_sec +
          ((double)openbuf.ru_stime.tv_usec)/1e6;
        open_u_short = open_u_short + open_u;
 open_s_short = open_s_short + open_s;
        close(fd);
      }
    for(i=0; i < 10000; i++) //short path already open
      {
        int fd1 = open("/etc/motd", O_RDONLY);
        struct rusage openbuf;
        fd = open("/etc/motd", O_RDONLY);
        getrusage(RUSAGE_SELF, &openbuf);
        open_u = (double)openbuf.ru_utime.tv_sec +
          ((double)openbuf.ru_utime.tv_usec)/1e6;
        open_s = (double)openbuf.ru_stime.tv_sec +
          ((double)openbuf.ru_stime.tv_usec)/1e6;
        open_u_short_open = open_u_short_open + open_u;
        open_s_short_open = open_s_short_open + open_s;
        close(fd);
        close(fd1);
      }
    double long_u;
    double long_s;
 double long_u2;
    double long_s2;
    double open_u_long = 0;
    double open_s_long = 0;
    for(i=0; i < 10000; i++) //long path varying already open and not
      {
        struct rusage longbuf;
        fd = open("../../dir1/dir2/dir3/dir4/dir5/random", O_RDONLY);
        getrusage(RUSAGE_SELF, &longbuf);
        struct rusage longbuf2;
        int fd2 = open("../../dir1/dir2/dir3/dir4/dir5/random", O_RDONLY);
        if (fd<0) perror("open");
        getrusage(RUSAGE_SELF, &longbuf2);
        long_u = (double)longbuf.ru_utime.tv_sec +
          ((double)longbuf.ru_utime.tv_usec)/1e6;
        long_u2 = (double)longbuf2.ru_utime.tv_sec +
          ((double)longbuf2.ru_utime.tv_usec)/1e6;
        long_s = (double)longbuf.ru_stime.tv_sec +
          ((double)longbuf.ru_stime.tv_usec)/1e6;
        long_s2 = (double)longbuf2.ru_stime.tv_sec +
          ((double)longbuf2.ru_stime.tv_usec)/1e6;
 open_u_long = open_u_long + long_u + long_u2;
        open_s_long = open_s_long + long_s + long_s2;
        close(fd);
        close(fd2);
      }
    printf("open user=%e\n", (open_u_short + open_u_short_open + open_u_long -
                              file_repeat_u)/30000);
    printf("open system=%e\n", (open_s_short + open_s_short_open + open_s_long
                                - file_repeat_s)/30000);
    // did it work?
    if (fd<0) perror("open");
    close(fd);
    printf("CIRCUMSTANCE: For the open call, long paths take significantly longer than short paths, and whether the file is open does not affect time as significantly.\
 A short path user time for example, takes between 5 and 7 e-02 while a long path user time takes between 9e-02 and 1.1e-01.\n\n");
    // how to allocate memory
    void *p;
    double sbrkfinalu = 0;
    double sbrkfinals = 0;
    for(i = 0; i < 50000; i++) //smaller sbrk
      {
        struct rusage brkbuf;
 p=sbrk(8024);
        getrusage(RUSAGE_SELF, &brkbuf);
        double u=(double)brkbuf.ru_utime.tv_sec
          + ((double)brkbuf.ru_utime.tv_usec)/1e6;
        sbrkfinalu = sbrkfinalu + u;
        double s=(double)brkbuf.ru_stime.tv_sec
          + ((double)brkbuf.ru_stime.tv_usec)/1e6;
        sbrkfinals = sbrkfinals + s;
        // did it work?
        if (p==NULL) perror("sbrk");
      }
    for(i = 0; i < 50000; i++) //larger sbrk
      {
        struct rusage brkbuf;
        p=sbrk(80240);
        getrusage(RUSAGE_SELF, &brkbuf);
        double u=(double)brkbuf.ru_utime.tv_sec
          + ((double)brkbuf.ru_utime.tv_usec)/1e6;
        sbrkfinalu = sbrkfinalu + u;
        double s=(double)brkbuf.ru_stime.tv_sec
 + ((double)brkbuf.ru_stime.tv_usec)/1e6;
        sbrkfinals = sbrkfinals + s;
        // did it work?
        if (p==NULL) perror("sbrk");
      }
    printf("sbreak user=%e\n", (sbrkfinalu - repeat_u)/100000);
    printf("sbreak system=%e\n", (sbrkfinals - repeat_s)/100000);
    printf("CIRCUMSTANCE: When the amount of memory is changed for sbrk, user time seems to have a positive correlation with an increase in number of bytes in sbrk whi\
le the variation in system time is scattered.\n\n");
}

