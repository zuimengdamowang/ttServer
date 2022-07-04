#pragma once
#include <sys/epoll.h>
#include <vector>

/*
Epoll类可以理解为对epfd的进一步封装，并定义了相关行为

*/

class Channel;
class Epoll {
public:
    Epoll();
    ~Epoll();

public:

    void UpdateChannel(Channel *channel);
    void DelChannel(Channel *channel);

    std::vector<Channel*> Poll(int timeout = -1);

private:
    int epfd_{-1};
    std::vector<struct epoll_event> events_;
};