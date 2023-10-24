#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define RD 0
#define WR 1

int get_first(int p[2],int *dst){
    close(p[WR]);
    if(read(p[RD],dst,4)==4){
        printf("prime %d\n",*dst);
        return 0;
    }
    return -1;
}

void primes(int lpipe[2]){
    int first;
    int data;
    close(lpipe[WR]);
    if(get_first(lpipe,&first)==0){
        int rp[2];
        pipe(rp);
        while(read(lpipe[RD],&data,4)==4){
            if(data%first){
                write(rp[WR],&data,4);
            }
        }
        int pid=fork();
        if(pid<0){
            printf("fork() error\n");
        }
        else if(pid==0){
            primes(rp);
        }
        else{
            close(lpipe[RD]);
            close(lpipe[WR]);
            close(rp[WR]);
            wait(0);
        }
    }
}

int main(int argc,char *argv[]){
    int fd[2];
    pipe(fd);
    for(int i=2;i<=35;i++){
        write(fd[WR],&i,sizeof(int));
    }
    int pid=fork();
    if(pid<0){
        printf("fork() error\n");
        exit(0);
    }
    // 父进程和子进程都要关闭管道的写端
    // 不然子进程会阻塞在read上，就无法结束
    else if(pid==0){
        // close(fd[WR]);
        primes(fd);
        // int data;
        // close(fd[WR]);
        // while(read(fd[RD],&data,4)==4){
        //     printf("%d\n",data);
        // }
    }
    else{
        close(fd[WR]);
        close(fd[RD]);
        wait(0);
    }
    exit(0);
}