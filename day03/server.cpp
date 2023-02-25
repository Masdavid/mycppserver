#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 100

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

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

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");  //参数是一个flag，一般设为0，详细参考man epoll

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));
    bzero(&ev, sizeof(ev));

    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;               //在代码中使用了ET模式，且未处理错误，在day12进行了修复，实际上接受连接最好不要用ET模式
    setnonblocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);  //将服务器socket fd添加到epoll

    while(true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");

        for(int i = 0; i < nfds; ++i) { 
            if(events[i].data.fd == sockfd) {         //新客户端连接
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr*)& clnt_addr, &clnt_addr_len);
                errif(clnt_sockfd == -1, "socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);    //将该客户端的socket fd添加到epoll

            } else if (events[i].events & EPOLLIN) {   //可读事件
                char buf[READ_BUFFER];
                while(true) {                   //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0) {
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));  
                    } else if(bytes_read == -1 && errno == EINTR) {  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { //非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if (bytes_read == 0) {  //EOF，客户端断开连接
                        printf("EOF,  client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);                    ////关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            } else {
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);
    return 0;
}