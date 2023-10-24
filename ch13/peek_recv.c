#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 30

int main(int argc,char *argv[]){
    int acpt_sock,recv_sock;
    struct sockaddr_in acpt_adr,recv_adr;
    int str_len,state;
    socklen_t recv_adr_sz;
    char buf[BUF_SIZE];
    acpt_sock=socket(AF_INET,SOCK_STREAM,0);
    memset(&acpt_adr,0,sizeof(acpt_adr));
    acpt_adr.sin_family=AF_INET;
    acpt_adr.sin_port=htons(atoi(argv[1]));
    acpt_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(acpt_sock,(struct sockaddr*)&acpt_adr,sizeof(acpt_adr));
    listen(acpt_sock,5);
    recv_adr_sz=sizeof(recv_adr);
    recv_sock=accept(acpt_sock,(struct sockaddr*)&recv_adr,&recv_adr_sz);
    while(1){
        // 尝试进行读取，其实就是看输入缓存中是否有数据，这个读取是非阻塞的
        str_len=recv(recv_sock,buf,sizeof(buf)-1,MSG_PEEK|MSG_DONTWAIT);
        if(str_len>0)
            break;
    }
    buf[str_len]=0;
    printf("buffering %d bytes:%s\n",str_len,buf);
    // 26行只是尝试读取，看输入缓存中是否有要读的数据
    // 这里是真的从缓存中把数据读走
    str_len=recv(recv_sock,buf,sizeof(buf)-1,0);
    buf[str_len]=0;
    printf("read again:%s\n",buf);
    close(acpt_sock);
    close(recv_sock);
    return 0;
}