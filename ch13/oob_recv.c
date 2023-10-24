#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUF_SIZE 30

void error_handler(char *buf){
    fputs(buf,stderr);
    fputc('\n',stderr);
    exit(1);
}

void urg_handler(int signo);

int acpt_sock;
int recv_sock;

int main(int argc,char *argv[]){
    struct sockaddr_in recv_adr,serv_adr;
    int str_len,state;
    socklen_t serv_adr_sz;
    struct sigaction act;
    char buf[BUF_SIZE];
    if(argc!=2)
        error_handler("missing parameter");
    act.sa_handler=urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    acpt_sock=socket(AF_INET,SOCK_STREAM,0);
    memset(&recv_adr,0,sizeof(recv_adr));
    recv_adr.sin_family=AF_INET;
    recv_adr.sin_port=htons(atoi(argv[1]));
    recv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(acpt_sock,(struct sockaddr*)&recv_adr,sizeof(recv_adr))==-1)
        error_handler("bind() error");
    listen(acpt_sock,5);
    serv_adr_sz=sizeof(serv_adr);
    recv_sock=accept(acpt_sock,(struct sockaddr*)&serv_adr,&serv_adr_sz);
    // 设置当前进程可以接收到recv_sock上的SIGIO和SIGURG信号
    fcntl(recv_sock,F_SETOWN,getpid());
    state=sigaction(SIGURG,&act,0);
    while((str_len=recv(recv_sock,buf,sizeof(buf),0))!=0){
        if(str_len==-1)
            continue;
        buf[str_len]=0;
        puts(buf);
    }
    close(recv_sock);
    close(acpt_sock);
    return 0;
}

void urg_handler(int signo){
    int str_len;
    char buf[BUF_SIZE];
    // OOB数据只能读取一个字节
    str_len=recv(recv_sock,buf,sizeof(buf)-1,MSG_OOB);
    buf[str_len]=0;
    printf("urgent message:%s\n",buf);
}