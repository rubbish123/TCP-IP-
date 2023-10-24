#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc,char *argv[]){
    fd_set reads,temps;
    int result,str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;
    FD_ZERO(&reads);
    // 监视0号描述符的输入事件
    // 0号文件描述符就是标准输入
    FD_SET(0,&reads);
    while(1){
        temps=reads;
        timeout.tv_sec=5;
        timeout.tv_usec=0;
        // 反复调用select
        result=select(1,&temps,0,0,&timeout);
        if(result==-1){
            puts("select() error");
            break;
        }
        else if(result==0){
            puts("time out");
        }
        else{
            // 如果0在temps中，说明0号文件描述符有输入事件发生
            if(FD_ISSET(0,&temps)){
                str_len=read(0,buf,BUF_SIZE);
                buf[str_len]=0;
                printf("message from console:%s\n",buf);
            }
        }
    }
    return 0;
}