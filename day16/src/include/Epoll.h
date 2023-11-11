#pragma once
#include "Macros.h"
#include <vector>

#include <sys/epoll.h>

class Channel;
class Epoll {
public:
    Epoll();
    ~Epoll();

    DISALLOW_COPY_AND_MOVE(Epoll);

    void UpdateChannel(Channel* ch);
    void DeleteChannel(Channel* ch);

    std::vector<Channel* > Poll(int timeout = -1);

private:
    int epfd_ {1};
    struct epoll_event* events_{nullptr};
};