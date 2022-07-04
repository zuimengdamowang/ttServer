#pragma once
#include <functional>



// Acceptor主要负责监听事件，并将返回的cfd添加到对应的sub-reactor

class Socket;
class Channel;
class EventLoop;
class Acceptor {    
public:
    Acceptor(EventLoop *reactor, const std::string &ip, uint16_t port);  
    ~Acceptor();

public:
    void SetDispatchCallback(std::function<void(Socket *)> const &cb);

private:
    // acceptor自己的行为（监听，然后调用accept函数，然后将返回的cfd添加到对应的sub-reactor）
    void AcceptConn();

private:
    EventLoop   *main_reactor_{nullptr};     // acceptor要了解自己属于哪个reactor
    Channel     *channel_{nullptr};    // acceptor要有自己的行为
    Socket      *socket_{nullptr};     // acceptor初始化时首先要进行socket的初始化，包括创建，绑定，监听
    std::function<void(Socket *)> dispatch_cb_;   // 用于将cfd添加到对应的sub-reactor
};