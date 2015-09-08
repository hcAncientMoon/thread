#include<stdarg.h>
#include "inc.h"
void Pthread_error_sys(int status, const char* fmt,...);
void Pthread_error_quit(int status, const char*fmt,...);
void Pthread_error_message(int status,const char*fmt,...);
static void Pthread_error_doit(int status,const char*fmt,int error_flag,int level,va_list args);
