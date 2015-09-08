#include"errors.h"
#include<syslog.h>

int is_deamon_proc = 0;

void Pthread_error_sys(int status,const char* fmt,...){
    va_list args;
    va_start(args,fmt);
    Pthread_error_doit(status,fmt,1,LOG_ERR|LOG_USER,args);
    exit(-1);

}
void Pthread_error_quit(int status,const char* fmt,...){
    va_list args;
    va_start(args,fmt);
    Pthread_error_doit(status,fmt,0,LOG_USER,args);
    exit(1);
}
void Pthread_error_message(int status , const char*fmt,...){
    va_list args;
    va_start(args,fmt);
    Pthread_error_doit(status,fmt,0,LOG_INFO|LOG_USER,args);
}

static void Pthread_error_doit(int status,const char*fmt,int error_flag,int level,va_list args){
    char message[MAXLINE+1];
    int cnt =vsnprintf(message,MAXLINE,fmt,args);
    if(error_flag)
        snprintf(message+cnt,MAXLINE-cnt,":%s",strerror(status));
    strcat(message,"\n");
    if(is_deamon_proc){
        syslog(level,message);
    }
    else{
        fflush(stdout);
        fputs(message,stderr);
        fflush(stderr);
    }
}
