#pragma once

#include "Macros.h"
#include <memory>
#include <functional>

class EventLoop;
class Socket;
class Channel;
class Acceptor {
public:
  explicit Acceptor(EventLoop* loop);
  ~Acceptor();

  DISALLOW_COPY_AND_MOVE(Acceptor);

  void AcceptConnection();
  void SetNewConnectionCallback(std::function<void(Socket*)> const& callback);

private:
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<Channel> channel_;
  std::function<void(Socket*)> new_connection_callback_;
};