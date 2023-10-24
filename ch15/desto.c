#include <stdio.h>
#include <fcntl.h>

int main(){
    FILE *fp;
    int fd=open("data.dat",O_WRONLY|O_CREAT|O_TRUNC);
    if(fd==-1){
        fputs("file open error",stdout);
        return -1;
    }
    // 把文件描述符转为FILE结构体
    fp=fdopen(fd,"w");
    fputs("network c programming\n",fp);
    fclose(fp);
    return 0;
}