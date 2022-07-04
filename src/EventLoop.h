#pragma once

/*
EventLoop就是一个reactor（main或者sub都可以）
可以理解为对Epoll的进一步封装，并提供相关行为

*/

class HeapTimer;
class Epoll;
class Channel;
class Server;
class ThreadPool;
class EventLoop {
public:
    EventLoop(Server *server);
    ~EventLoop();

public:
    void Loop();  // 循环监听

    void UpdateChannelToEpoll(Channel *cha);
    void DelChannelfromEpoll(Channel *cha);

    Epoll       *GetEpoll();
    Server      *GetServer();
    HeapTimer   *GetHeapTimer();


public:
    int         timeout_MS_{1000};

private:
    Epoll       *epoll_{nullptr};    // 内部封装epoll_
    Server      *server_{nullptr};   // 需要知道自己属于那个服务器
    ThreadPool  *threadpool_{nullptr};  // 一个sub-reactor有一个线程池
    bool        is_reactor_stop_{true}; 
    HeapTimer   *timer_{nullptr};  // 一个sub-reactor有一个属于自己的定时器
    
};
