#include "Acceptor.h"
#include <sys/epoll.h>
#include "Server.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"

#include <iostream>



Acceptor::Acceptor(EventLoop *reactor, const std::string &ip, uint16_t port) : 
    main_reactor_(reactor) 
{
    
    // socket相关的初始化
    socket_ = new Socket;
    InetAddress *serv_addr = new InetAddress(ip.c_str(), port);
    socket_->Bind(serv_addr);
    socket_->Listen();

    // 设置channel
    channel_ = new Channel(socket_->GetFd(), main_reactor_);
    channel_->SetEvents(EPOLLIN | EPOLLPRI);
    channel_->SetReadCallback(std::bind(&Acceptor::AcceptConn, this));
    channel_->UpdateChannelToReactor();
    
}

Acceptor::~Acceptor(){
    if (channel_) delete channel_;
    if (socket_)  delete socket_;
}

void Acceptor::AcceptConn() {
    InetAddress *clnt_addr = new InetAddress;  // 客户端的信息
    Socket *c_socket = new Socket(socket_->Accept(clnt_addr));  // 调用accept函数，并返回相应的cfd
    dispatch_cb_(c_socket);  // 将cfd添加到对应的sub-reactor
    delete clnt_addr;
}

void Acceptor::SetDispatchCallback(std::function<void(Socket *)> const &cb) {
    dispatch_cb_ = cb;
}

