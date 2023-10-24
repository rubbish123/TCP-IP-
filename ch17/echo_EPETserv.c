#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50

void setnonblockingmode(int fd){
    // 把文件描述符改为非阻塞
    int flag=fcntl(fd,F_GETFL,0);
    fcntl(fd,F_SETFL,flag|O_NONBLOCK);
}

void error_handler(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc,char *argv[]){
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    socklen_t adr_sz;
    int str_len,i;
    char buf[BUF_SIZE];

    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd,event_cnt;

    if(argc!=2)
        error_handler("miss parameter");
    serv_sock=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_port=htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
        error_handler("bind() error");
    if(listen(serv_sock,5)==-1)
        error_handler("listen() error");
    // 创建epoll例程空间
    epfd=epoll_create(EPOLL_SIZE);
    // 申请存储发生变化的文件描述符的空间
    ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
    // 把负责建立连接的sock注册进去
    // 下面这种写法是边缘触发 ET:Edge Trigger
    setnonblockingmode(serv_sock);
    event.events=EPOLLIN;
    event.data.fd=serv_sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);
    while(1){
        // 获取发生变化的文件描述符的集合
        /*
            条件触发：只要输入缓冲里的数据没读完，就反复调用epoll_wait注册事件
        */
        event_cnt=epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
        if(event_cnt==-1){
            puts("epoll wait() error");
            break;
        }
        puts("return epoll_wait");
        // 遍历所有的事件
        for(int i=0;i<event_cnt;i++){
            // 如果是serv_sock有事件发生，那说明是要建立新连接
            if(ep_events[i].data.fd==serv_sock){
                // 建立连接并将文件描述符注册到epoll空间
                adr_sz=sizeof(clnt_adr);
                clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
                setnonblockingmode(clnt_sock);
                event.events=EPOLLIN|EPOLLET;
                event.data.fd=clnt_sock;
                epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);
                printf("connected client:%d\n",clnt_sock);
            }
            else{
                while(1){
                    // 如果是clnt_sock有事件发生,那说明要读取数据
                    str_len=read(ep_events[i].data.fd,buf,BUF_SIZE);
                    // 没读到
                    if(str_len==0){
                        // 关闭连接，把文件描述符从epoll空间删除
                        epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
                        close(ep_events[i].data.fd);
                        printf("closed client:%d\n",ep_events[i].data.fd);
                    }
                    else if(str_len<0){
                        if(errno==EAGAIN)
                            break;
                    }
                    else{
                        // 读到了就写回客户端
                        write(ep_events[i].data.fd,buf,str_len);
                    }
                }
                
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}
