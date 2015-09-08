#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "inc.h"

// defines
#define CREWSIZE 4
typedef struct work_tag{
    struct work_tag* next;
    char* path;
    char* string;
}work_t,*work_p;

typedef struct worker_tag{
    int index;
    pthread_t tid;
    struct crew_tag* crew;
}worker_t,*worker_p;

typedef struct crew_tag{
    int crew_size;
    worker_t workers[CREWSIZE];
    long work_cnt;
    work_t *first_work,*last_work;
    pthread_mutex_t mutex;
    pthread_cond_t done;
    pthread_cond_t go;
}crew_t,*crew_p;
size_t path_max;
size_t name_max;



void* work_loop(void* args){

    worker_p mine = (worker_p)args;
    crew_p crew = mine->crew;
    struct stat file_stat;
    struct dirent *entry;
    work_p  work;
    entry = (struct dirent*)malloc(sizeof(struct dirent)+name_max);
    Pthread_mutex_lock(&crew->mutex);
    while(crew->work_cnt==0){
        Pthread_cond_wait(&crew->go,&crew->mutex);
    }
    Pthread_mutex_unlock(&crew->mutex);
    while(1){
        Pthread_mutex_lock(&crew->mutex);
        while(crew->first_work==NULL)
            Pthread_cond_wait(&crew->go,&crew->mutex);
        printf("woker %d works on :#%lx ,conunt is %d\n", mine->index,crew->first_work,crew->work_cnt);
        work = crew->first_work;
        crew->first_work = work->next;
        if(crew->first_work==NULL)
            crew->last_work = NULL;
        printf("woker %d took #%lx ,leaave first is #%lx last is #%lx\n", mine->index,work,crew->first_work,crew->last_work);
        Pthread_mutex_unlock(&crew->mutex);

        if(lstat(work->path,&file_stat)<0){
            printf("%s error: %s\n",work->path,strerror(errno));
        }
        if(S_ISLNK(file_stat.st_mode)){
            printf("%s is a link\n", work->path);
        }
        else if (S_ISDIR(file_stat.st_mode)){
            DIR* directory = opendir(work->path);
            struct dirent* result;
            if(directory==NULL){
                fprintf(stderr, "open %s errro:%s\n", work->path,strerror(errno));
                continue;
            }
            while(1){
                int status = readdir_r(directory,entry,&result);
                if(status!=0){
                    fprintf(stderr, "unable to open %s,error:%s\n", work->path,strerror(status));
                    break;
                }
                if(result==NULL)
                    break;
                if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)
                    continue;
                work_p new_work= (work_p)malloc(sizeof(work_t));
                new_work->string = work->string;
                new_work->path = (char*) malloc(path_max);
                strcpy(new_work->path,work->path);
                strcat(new_work->path,"/");
                strcat(new_work->path,entry->d_name);
                new_work->next= NULL;
                Pthread_mutex_lock(&crew->mutex);
                if(crew->first_work==NULL){
                    crew->first_work=new_work;
                    crew->last_work = new_work;
                }else{
                    crew->last_work->next = new_work;
                    crew->last_work = new_work;
                }
                crew->work_cnt++;
                Pthread_cond_signal(&crew->go);
                Pthread_mutex_unlock(&crew->mutex);
            }
            closedir(directory);
        }
        else if (S_ISREG(file_stat.st_mode)){
            FILE* file;
            char buf[MAXLINE],*fbuf;
            if((file = fopen(work->path,"r"))==NULL){
                fprintf(stderr, "can not open %s:%s\n", work->path,strerror(errno));
                continue;
            }
            else{
                while(1){
                   if((fbuf= fgets(buf,MAXLINE,file))==NULL){
                        if(feof(file))
                            break;
                        else if(ferror(file)){
                            fprintf(stderr, "read %s error:%s\n", work->path,strerror(errno));
                            break;
                        }
                   }
                   if(strstr(buf,work->string)!=NULL){
                        printf("found %s in %s\n",work->string,work->path);
                        break;
                   }
                }
                fclose(file);

            }
        }
        else{
            fprintf(stderr, "%s's type is %s\n", work->path,S_ISFIFO(file_stat.st_mode)?"FIFO":
                                                            S_ISCHR(file_stat.st_mode)?"CHAR":
                                                            S_ISBLK(file_stat.st_mode)?"BLK":
                                                            S_ISSOCK(file_stat.st_mode)?"SOCK":"unknown");

        }
        free(work->path);
        free(work);
        Pthread_mutex_lock(&crew->mutex);
        crew->work_cnt--;
        if(crew->work_cnt<=0){
            Pthread_cond_broadcast(&crew->done);
            Pthread_mutex_unlock(&crew->mutex);
            break;
        }
        Pthread_mutex_unlock(&crew->mutex);

    }
    free(entry);
    return NULL;
}
int crew_create(crew_p crew,int crew_size){
    int worker_index;
    pthread_t tid;
    if(crew_size>CREWSIZE)
        return EINVAL;
    Pthread_mutex_init(&crew->mutex,NULL);
    Pthread_cond_init(&crew->done,NULL);
    Pthread_cond_init(&crew->go,NULL);
    crew->crew_size = crew_size;
    crew->work_cnt =0;
    crew->first_work=crew->last_work= NULL;
    for(worker_index=0;worker_index<crew_size;worker_index++){
        crew->workers[worker_index].index = worker_index;
        crew->workers[worker_index].crew =crew;
        Pthread_create(&tid,NULL,work_loop,&crew->workers[worker_index]);
        crew->workers[worker_index].tid = tid;
    }
    return 0;
}
int crew_start(crew_p crew,char* path,char* string){
    Pthread_mutex_lock(&crew->mutex);
    while(crew->work_cnt>0){
        Pthread_cond_wait(&crew->done,&crew->mutex);
    }
    work_p work = (work_p)malloc(sizeof(work));
    errno =0;
    name_max = pathconf(path,_PC_NAME_MAX);
    if((int)name_max==-1){
        if(errno==0)
            name_max = 2560;
        else{
            printf("%s\n",strerror(errno));
            exit(-1);
        }
    }
    path_max = pathconf(path,_PC_PATH_MAX);
    if((int)path_max==-1){
        if(errno==0)
            path_max=102400;
        else{
            printf("%s\n",strerror(errno));
            exit(-1);
        }
    }
    path_max++;
    name_max++;

    work->string =string;
    work->path = (char*)malloc(path_max);
    strcpy(work->path,path);
    work->next = NULL;
    if(crew->first_work==NULL){
        crew->first_work = work;
        crew->last_work = work;
    }else{
        crew->last_work->next = work;
        crew->last_work = work;
    }
    crew->work_cnt ++;
    Pthread_cond_signal(&crew->go);
    while(crew->work_cnt>0){
        Pthread_cond_wait(&crew->done,&crew->mutex);
    }
    return 0;
}
int main(int argc, char *argv[])
{
    if(argc<3)
        exit(-1);
    crew_t crew;
    if(crew_create(&crew,3)!=0){
        printf("crew create error\n");
        exit(-1);
    }
    crew_start(&crew,argv[2],argv[1]);

    return 0;
}

