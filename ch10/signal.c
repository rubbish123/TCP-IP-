#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){
    if(sig==SIGALRM)
        puts("time out");
    alarm(2);
}
void keycontrol(int sig){
    if(sig==SIGINT)
        puts("\nctrl+C pressed");
}

int main(){
    // 把信号和对应的处理函数绑定
    signal(SIGALRM,timeout);
    signal(SIGINT,keycontrol);
    alarm(2);
    for(int i=0;i<3;i++){
        puts("wait...");
        sleep(100);
    }
    return 0;
}