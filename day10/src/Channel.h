#pragma once
#include <sys/epoll.h>
#include <functional>

class EventLoop;
class Channel
{
private:
    // Epoll* ep;
    EventLoop* loop;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;

    std::function<void()> callback;
public:
    Channel(EventLoop* _loop, int _fd);
    ~Channel();

    void handleEvent();    
    void enabelReading();
    
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    void setRevents(uint32_t);
    void setCallback(std::function<void()>);

};
