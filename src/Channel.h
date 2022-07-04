#pragma once
#include <functional>
#include <sys/epoll.h>


/*
Channel 可以理解为文件描述符fd的进一步抽象
即内部封装了fd，同时也描述了它的一系列行为。

例如：
监听描述符lfd的行为是监听，并调用accept函数返回cfd
通信描述符cfd的行为是用于通信。
这些行为被设置在了内部的回调函数上，
当这个channel实例被创建时会设置对应的回调函数。
注意：一个channel实例需要知道它对应的epoll（eventloop）是谁，
     这样才能将channel添加到相应的epoll树中去。
*/

class EventLoop;
class Channel
{
public:
    Channel(int fd, EventLoop *loop);  // 内部封装了fd，需要知道对应的eventloop（epoll）
    ~Channel();

public:
    void HandleEvent();  // 执行channel行为

    int      GetFd();
    uint32_t GetEvents();
    uint32_t GetReadyEvents();
    bool     GetIsInEpoll();

    void SetIsInEpoll(bool cond);
    void SetReadCallback(std::function<void()> const &cb);
    void SetWriteCallback(std::function<void()> const &cb);
    void SetEvents(uint32_t events);
    void SetReadyEvents(uint32_t events);
    void SetET();  // 设置为ET模式
    
    void UpdateChannelToReactor();  // 将channel添加到相应的epoll中去，以至于可以被监听

private:
    EventLoop *loop_{nullptr};    // 需要知道eventloop（epoll）是谁
    int        fd_{-1};           // 内部封装的fd
    uint32_t   events_;           // fd对应的初始事件，主要是描述fd可以做哪些行为
    uint32_t   ready_events_;     // 描述fd再从epoll返回之后的行为
    bool       is_in_epoll_{false};  // 标记这个channel是否在epoll中

    std::function<void()> read_cb_;    // 读行为
    std::function<void()> write_cb_;   // 写行为
    

};