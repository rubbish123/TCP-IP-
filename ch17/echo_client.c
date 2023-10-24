#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

int main(int argc,char *argv[]){
    int sd;
    char buf[BUF_SIZE];
    int str_len;
    int read_cnt;
    struct sockaddr_in serv_adr;
    sd=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_port=htons(atoi(argv[2]));
    serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
    connect(sd,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
    while(1){
        fputs("insert message(q to quit):",stdout);
        fgets(buf,BUF_SIZE,stdin);
        if(!strcmp(buf,"q\n")||!strcmp(buf,"Q\n"))
            break;
        write(sd,buf,strlen(buf));
        str_len=read(sd,buf,BUF_SIZE-1);
        buf[BUF_SIZE]=0;
        printf("message from server:%s\n",buf);
    }
    close(sd);
    return 0;
}