#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char *argv[]){
    int sock;
    struct sockaddr_in send_adr;
    sock=socket(AF_INET,SOCK_STREAM,0);
    memset(&send_adr,0,sizeof(send_adr));
    send_adr.sin_family=AF_INET;
    send_adr.sin_addr.s_addr=inet_addr(argv[1]);
    send_adr.sin_port=htons(atoi(argv[2]));
    connect(sock,(struct sockaddr*)&send_adr,sizeof(send_adr));
    write(sock,"123",strlen("123"));
    close(sock);
    return 0;
}