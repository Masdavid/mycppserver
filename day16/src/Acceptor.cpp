#include "Acceptor.h"

#include <utility>
#include <cstdio>

#include "Channel.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop* loop): sock_(nullptr), channel_(nullptr) {
  sock_ = std::make_unique<Socket>();
  InetAddress* addr = new InetAddress("127.0.0.1", 1234);
  sock_->Bind(addr);
  //acceptor use blocking socket
  sock_->Listen();
  
  channel_ = std::make_unique<Channel>(loop, sock_->GetFd());
  std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
  channel_->SetReadCallback(cb);
  channel_->EnableRead();
}

Acceptor::~Acceptor() {
}

void Acceptor::AcceptConnection() {
  InetAddress* clnt_addr = new InetAddress();
  Socket* clnt_sock = new Socket(sock_->Accept(clnt_addr));
  printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp(), clnt_addr->GetPort());

  clnt_sock->SetNonBlocking();
  new_connection_callback_(clnt_sock);       //建立连接之后的操作交给回调，acceptor不关心
  delete clnt_addr;
}

void Acceptor::SetNewConnectionCallback(std::function<void(Socket*)> const& callback) {
  new_connection_callback_ = callback;       //漏洞，if AcceptConnection比SetNewConnectionCallback早调用，会出问题
}