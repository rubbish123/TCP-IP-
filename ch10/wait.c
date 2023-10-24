#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc,char *argv[]){
    int status;
    pid_t pid=fork();
    if(!pid){
        return 3;
    }
    else{
        printf("child pid:%d\n",pid);
        pid=fork();
        if(!pid){
            exit(7);
        }
        else{
            printf("child pid:%d\n",pid);
            // 手动回收子进程并获取其退出状态
            // wait回收子进程会阻塞
            // 使用waitpid并设置WNOHANG就不会阻塞
            wait(&status);
            if(WIFEXITED(status))
                printf("child send one:%d\n",WEXITSTATUS(status));
            wait(&status);
            if(WIFEXITED(status))
                printf("child send two:%d\n",WEXITSTATUS(status));
            sleep(30);
        }
        return 0;
    }
}