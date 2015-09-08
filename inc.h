#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#define MAXLINE 1024


int Pthread_create(pthread_t* tid,const pthread_attr_t* attr,void*(*run)(void*),void*args);
int Pthread_detach(pthread_t tid);
int Pthread_join(pthread_t tid,void**retval);
void Pthread_exit(void* retval);


int Pthread_mutex_init(pthread_mutex_t* mutex,const pthread_mutexattr_t *attr);
int Pthread_mutex_lock(pthread_mutex_t* mutex);
int Pthread_mutex_unlock(pthread_mutex_t* mutex);
int Pthread_mutex_trylock(pthread_mutex_t* mutex);

int Pthread_cond_init(pthread_cond_t* mutex,const pthread_condattr_t* attr);
int Pthread_cond_wait(pthread_cond_t* cond,pthread_mutex_t* mutex);
int Pthread_cond_timedwait(pthread_cond_t* cond,pthread_mutex_t* mutex,struct timespec* abs);
int Pthread_cond_signal(pthread_cond_t* cond);
int Pthread_cond_broadcast(pthread_cond_t*cond);
