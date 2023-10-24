#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUF_SIZE 30
void read_childproc(int sig);

int main(int argc,char *argv[]){
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len,state;
    char buf[BUF_SIZE];
    act.sa_handler=read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGCHLD,&act,0);
    serv_sock=socket(AF_INET,SOCK_STREAM,0);
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_port=htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
    listen(serv_sock,10);
    while(1){
        adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
        if(clnt_sock==-1)
            continue;
        else
            puts("new client connected...");
        pid=fork();
        if(pid==-1){
            /*
                因为fork会复制文件描述符，
                所以在父子进程中需要close掉不需要的文件描述符
                否则最后会使得套接字无法销毁
                需要注意套接字是操作系统所有而不是进程所有
                因此fork不会复制套接字
            */
            close(clnt_sock);
            continue;
        }
        if(pid==0){
            close(serv_sock);
            while((str_len=read(clnt_sock,buf,BUF_SIZE))!=0){
                write(clnt_sock,buf,str_len);
            }
            close(clnt_sock);
            puts("client disconnected");
            return 0;
        }
    }
    return 0;
}

void read_childproc(int sig){
    int status;
    pid_t pid=waitpid(-1,&status,WNOHANG);
    printf("remove proc id:%d\n",pid);
}