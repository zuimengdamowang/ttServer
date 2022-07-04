#include "Server.h"
#include "util.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "Acceptor.h"
#include "Connection.h"
#include "Socket.h"
#include "Log.h"

#include <iostream>



Server::Server(const std::string &ip, uint16_t port) :
    ip_(ip), port_(port), is_stop_(false)
{   
    // main-reactor
    main_reactor_ = new Reactor(this);
    
    // Acceptor
    acceptor_ = new Acceptor(main_reactor_, ip_, port_);
    acceptor_->SetDispatchCallback(std::bind(&Server::Dispatch, this, std::placeholders::_1));

    // sub-reactors
    unsigned int size = std::thread::hardware_concurrency();
    for (unsigned int i = 0; i < size; ++i) {
        sub_reactors_.emplace_back(new Reactor(this));
    } 
    // 一个sub-reactor对应一个线程
    for (unsigned int i = 0; i < size; ++i) {
        std::thread(std::bind(&Reactor::Loop, sub_reactors_[i])).detach();
    }
    // 开启日志系统
    LOG_INFO("========== Server init ==========");
    LOG_INFO("IP: " + ip_ + ", Port: " + std::to_string(port_));
}


Server::~Server() {
    if (main_reactor_) delete main_reactor_;
}


std::string Server::GetIp () {return ip_;}

uint16_t Server::GetPort() {return port_;}

Server::Reactor * 
Server::GetMainReator() {
    return main_reactor_;
}


void
Server::Dispatch(Socket *clnt_sock) {
    int cfd = clnt_sock->GetFd();
    ErrorIf(-1 == cfd, "Dispatch cfd == -1, error");
    unsigned int idx = rand() % sub_reactors_.size();   // 指名要分配到那个sub-reactor
    Connection *conn = new Connection(sub_reactors_[idx], cfd);  // 在初始化conn的时候就已经将cfd及其对应的行为（channel）添加到epoll树中了
    conn->SetProcessCallback(process_conn_cb_);
    conn->SetDeleteConnCallback(std::bind(&Server::DeleteConn, this, std::placeholders::_1));
    conns_[cfd] = conn;
}



void
Server::DeleteConn(Connection *conn) {
    assert(conn != nullptr);
    conn->GetEventLoop()->DelChannelfromEpoll(conn->GetChannel());  // 从epoll中删除一个channel
    if (conns_.count(conn->GetFd()) > 0) {
        conns_.erase(conn->GetFd());
    }
    delete conn;  // 释放读写缓存 + 关闭cfd 
}


void
Server::Launch() {
    // 启动main reactor
    main_reactor_->Loop();
}


void Server::SetProcessConnCallback(std::function<void(Connection *)> const &cb) {
    process_conn_cb_ = cb;
}





