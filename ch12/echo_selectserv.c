#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

void error_handler(char *buf){
    fputs(buf,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc,char *argv[]){
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    struct timeval timeout;
    fd_set reads,cpy_reads;
    socklen_t adr_sz;
    int fd_max,str_len,fd_num,i;
    char buf[BUF_SIZE];
    serv_sock=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_port=htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
        error_handler("bind() error");
    if(listen(serv_sock,5)==-1)
        error_handler("listen() error");
    FD_ZERO(&reads);
    FD_SET(serv_sock,&reads);
    fd_max=serv_sock;

    while(1){
        cpy_reads=reads;
        timeout.tv_sec=5;
        timeout.tv_usec=0;
        fd_num=select(fd_max+1,&cpy_reads,0,0,&timeout);
        if(fd_num==-1){
            error_handler("select() error");
            break;
        }
        if(fd_num==0){
            fputs("time out \n",stdout);
            continue;
        }
        // 遍历找到有事件发生的文件描述符
        for(i=0;i<fd_max+1;i++){
            if(FD_ISSET(i,&cpy_reads)){
                // 如果serv_sock有事件，表明有新连接要建立
                if(i==serv_sock){
                    adr_sz=sizeof(clnt_adr);
                    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
                    // 把新连接加到监视列表并更新fd_max
                    FD_SET(clnt_sock,&reads);
                    if(fd_max<clnt_sock)
                        fd_max=clnt_sock;
                    printf("connected client:%d\n",clnt_sock);
                }
                // 如果不是serv_sock，那就是之前建立的连接有读事件
                else{
                    str_len=read(i,buf,BUF_SIZE);
                    if(str_len==0){
                        FD_CLR(i,&reads);
                        close(i);
                        printf("closed client:%d\n",i);
                    }
                    else{
                        write(i,buf,str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}