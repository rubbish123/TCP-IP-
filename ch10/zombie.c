#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc,char *argv[]){
    pid_t pid=fork();
    if(pid==0){
        puts("i am child process");
    }
    else{
        printf("child process ID:%d\n",pid);
        sleep(30);
    }
    // 子进程比父进程先结束，父进程又没有手动回收，子进程就成为僵尸进程
    if(pid==0)
        puts("end child process");
    else
        puts("end parent process");
    return 0;
}