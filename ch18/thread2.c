#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thread_main(void *arg);

int main(int argc,char *argv[]){
    pthread_t t_id;
    int thread_param=5;
    void *thr_ret;
    pthread_create(&t_id,NULL,thread_main,(void*)&thread_param);
    // 等待线程执行结束
    // thr_ret 接收线程函数结束后返回的参数
    // thr_ret:void*，因为是传入参数，所以要再取个地址
    pthread_join(t_id,&thr_ret);
    printf("thread return message:%s\n",(char*)thr_ret);
    free(thr_ret);
    return 0;
}
void *thread_main(void *arg){
    int cnt=*((int*)arg);
    char *msg=(char*)malloc(sizeof(char)*50);
    strcpy(msg,"hello,I am thread\n");
    for(int i=0;i<cnt;i++){
        sleep(1);
        puts("running thread");
    }
    return (void*)msg;
}