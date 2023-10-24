#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define BUF_SIZE 30

int main(){
    int fds[2];
    char str1[]="who are you";
    char str2[]="thank you for your message";
    char buf[BUF_SIZE];
    pid_t pid;
    pipe(fds);
    pid=fork();
    if(pid==0){
        write(fds[1],str1,sizeof(str1));
        /*
            这里sleep是为了留出时间让父进程把管道内的数据读出
            因为管道内的数据是无主数据，谁都可以read
            这里不sleep的话，子进程就会把管道内的数据读走
            管道里没数据，父进程再读的话就会阻塞
        */
        // sleep(2);
        read(fds[0],buf,BUF_SIZE);
        printf("child proc output:%s\n",buf);
    }
    else{
        read(fds[0],buf,BUF_SIZE);
        printf("parent proc output:%s\n",buf);
        write(fds[1],str2,sizeof(str2));
        sleep(3);
    }
    return 0;
}