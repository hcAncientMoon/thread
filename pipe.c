#include"inc.h"
#include "errors.h"

typedef struct stage_tag{
    int data;
    int can_process_flag;
    pthread_mutex_t mutex;
    pthread_cond_t can_process_cond;
    pthread_cond_t available;
    struct stage_tag* next;
} stage_t;
typedef struct pipeline_tag{
    pthread_mutex_t mutex;
    stage_t* head;
    stage_t* tail;
} pipeline_t;

int send_next(stage_t* next,int data){
    Pthread_mutex_lock(&next->mutex);
    while(!next->can_process_flag){
        Pthread_cond_wait(&next->can_process_cond,&next->mutex);
    }
    next->data = data;
    next->can_process_flag = 0;
    Pthread_cond_signal(&next->available);
    return Pthread_mutex_unlock(&next->mutex);
}
void*pipe_stage(void* args){
    stage_t* stage = (stage_t*)args;
    Pthread_mutex_lock(&stage->mutex);
    while(1){
        while(stage->can_process_flag){
            Pthread_cond_wait(&stage->available,&stage->mutex);
        }
        send_next(stage->next,stage->data+1);
        stage->can_process_flag = 1;
        Pthread_cond_signal(&stage->can_process_cond);
    }
}
void pipe_create(pipeline_t* pip,int stage_cnt){
    if(pip==NULL)
        pip = (pipeline_t*)malloc(sizeof(pipeline_t));
    Pthread_mutex_init(&pip->mutex,NULL);
    stage_t**link = &pip->head,*new_stage,*stage;
    int cnt;
    for( cnt=0;cnt<stage_cnt;cnt++){
        new_stage = (stage_t*)malloc(sizeof(stage_t));
        Pthread_mutex_init(&new_stage->mutex,NULL);
        Pthread_cond_init(&new_stage->can_process_cond,NULL);
        Pthread_cond_init(&new_stage->available,NULL);
        new_stage->can_process_flag =1;
        *link = new_stage;
        link=&new_stage->next;
    }
    pip->tail = new_stage;
    *link = NULL;
    pthread_t tid;
    for(stage = pip->head;stage->next!=NULL;stage=stage->next){
        Pthread_create(&tid,NULL,pipe_stage,stage);
    }
}
int pipe_start(pipeline_t* pip,long value){
    if(pip==NULL)
        exit(-1);
    Pthread_mutex_lock(&pip->mutex);
    send_next(pip->head,value);
    Pthread_mutex_unlock(&pip->mutex);
    return 0;
}
int pipe_result(pipeline_t* pip,long* value){
    if(pip==NULL)
        exit(-1);
    Pthread_mutex_lock(&pip->mutex);
    stage_t* last = pip->tail;
    Pthread_mutex_unlock(&pip->mutex);
    Pthread_mutex_lock(&last->mutex);
    while(last->can_process_flag){
        Pthread_cond_wait(&last->available,&last->mutex);
    }
    last->can_process_flag=1;
    *value= last->data+1;
    Pthread_cond_signal(&last->can_process_cond);
    Pthread_mutex_unlock(&last->mutex);
    return 1;
}
int main(int argc, char *argv[])
{
    long result=0;
    pipeline_t pip;
    pipe_create(&pip,10);
    pipe_start(&pip,1);
    pipe_start(&pip,10);
    pipe_result(&pip,&result);
    printf("%d\n", (int)result);
    pipe_result(&pip,&result);
    printf("%d\n", (int)result);
    return 0;
}
