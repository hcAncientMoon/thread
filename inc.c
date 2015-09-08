#include "inc.h"
#include "errors.h"
#define check_status(status,str) {if(status!=0)Pthread_error_sys(status,str);}
int Pthread_create(pthread_t* tid,const pthread_attr_t* attr,void*(*run)(void*),void*args){
    int status = pthread_create(tid,attr,run,args);
    check_status(status,"pthrad_create");
    return status;
}
int Pthread_detach(pthread_t tid){
    int status=pthread_detach(tid);
    check_status(status,"pthread_detach");
    return status;
}
int Pthread_join(pthread_t tid,void**retval){
    int status = pthread_join(tid,retval);
    check_status(status,"pthrad_join");
    return status;
}
void Pthread_exit(void* retval){
    pthread_exit(retval);
}
int Pthread_mutex_init(pthread_mutex_t* mutex,const pthread_mutexattr_t *attr){
    int status = pthread_mutex_init(mutex,attr);
    check_status(status,"pthread_mutex_init");
    return status;

}
int Pthread_mutex_lock(pthread_mutex_t* mutex){
    int status = pthread_mutex_lock(mutex);
    check_status(status,"pthread_mutex_lock");
    return status;
}
int Pthread_mutex_unlock(pthread_mutex_t* mutex){
    int status = pthread_mutex_unlock(mutex);
    check_status(status,"pthread_mutex_unlock");
    return status;
}
int Pthread_mutex_trylock(pthread_mutex_t* mutex){
    int status = pthread_mutex_trylock(mutex);
    check_status(status,"pthread_mutex_trylock");
    return status;
}

int Pthread_cond_init(pthread_cond_t* mutex,const pthread_condattr_t* attr){
    int status = pthread_cond_init(mutex,attr);
    check_status(status,"pthread_cond_init");
    return status;
}
int Pthread_cond_wait(pthread_cond_t* cond,pthread_mutex_t* mutex){
    int status = pthread_cond_wait(cond,mutex);
    check_status(status,"pthread_cond_wait");
    return status;
}
int Pthread_cond_timedwait(pthread_cond_t* cond,pthread_mutex_t* mutex,struct timespec* abs){
    int status=pthread_cond_timedwait(cond,mutex,abs);
    check_status(status,"pthread_cond_timedwait");
    return status;

}
int Pthread_cond_signal(pthread_cond_t* cond){
    int status = pthread_cond_signal(cond);
    check_status(status,"pthread_cond_signal");
    return status;
}
int Pthread_cond_broadcast(pthread_cond_t* cond){
    int status = pthread_cond_broadcast(cond);
    check_status(status,"pthread_cond_broadcast");
    return status;
}
