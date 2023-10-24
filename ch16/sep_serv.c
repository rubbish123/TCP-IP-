#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc,char *argv[]){
    int serv_sock,clnt_sock;
    FILE *readfp;
    FILE *writefp;
    struct sockaddr_in serv_adr,clnt_adr;
    socklen_t clnt_adr_sz;
    char buf[BUF_SIZE]={0,};
    serv_sock=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_port=htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);

    bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
    listen(serv_sock,5);
    clnt_adr_sz=sizeof(clnt_adr);
    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
    // 分离读写流
    readfp=fdopen(clnt_sock,"r");
    writefp=fdopen(clnt_sock,"w");

    fputs("from server:Hi client?\n",writefp);
    fputs("i love all of the world\n",writefp);
    fputs("you are awesome\n",writefp);
    fflush(writefp);

    // 这里虽然看起来只关闭了写端，但其实会把套接字关闭
    // 因为无论是读端还是写端都是基于同一个文件描述符创建的
    // 对哪个FILE结构体调用fclose都会关闭文件描述符
    // 下面的fgets是收不到来自客户端的字符串的
    fclose(writefp);
    fgets(buf,sizeof(buf),readfp);
    fputs(buf,stdout);
    fclose(readfp);
    return 0;
}