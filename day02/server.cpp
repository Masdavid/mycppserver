#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

int main() {
    //第一步，创建服务端的监听socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr)); //effective C++ 04

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    //第二步，服务端程序绑定地址和端口到socket上
    errif(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socker bind error");


    //第三步，把socker设置为监听模式
    errif(listen(sockfd, SOMAXCONN) == -1, "socker listen error");

    //第四步，接受客户端的连接，accpet会阻塞
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));
    int clnt_sockfd = accept(sockfd, (sockaddr*)& clnt_addr, &clnt_addr_len);
    errif(clnt_sockfd == -1, "socket accept error");

    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    while(true) {
        char buf[1024];  //定义缓冲区
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf)); //从客户端socket读到缓冲区，返回已读数据大小
        if(read_bytes > 0) {
            printf("message from client fd %d: %s\n", clnt_sockfd, buf);  
            write(clnt_sockfd, buf, sizeof(buf));
        } else if (read_bytes == 0) {
            printf("client fd %d disconnected\n", clnt_sockfd);  //read返回0，表示EOF
            close(clnt_sockfd);
            break;
        } else if (read_bytes == -1) {   //read返回-1，表示发生错误，按照上文方法进行错误处理
            close(clnt_sockfd);
            errif(true, "socket read error");  
        }
    }
    close(sockfd);
    return 0;
}