#pragma once

/*
实现一个基于主从Reactor多线程模式的服务器

*/

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>
#include <memory>

class Log;
class Socket;
class EventLoop;
class Acceptor;
class Connection;
class ThreadPool;
class Server {
public:
    using Reactor = EventLoop;

public:
    Server(const std::string &ip, uint16_t port);
    ~Server();

public:
    void Launch();

    std::string GetIp();
    uint16_t    GetPort();
    Reactor *   GetMainReator();

    void SetProcessConnCallback(std::function<void(Connection *)> const &cb);

private:
    void Dispatch(Socket *clnt_sock);  // 负责将一个cfd分配到相应的sub-reactor（即acceptor里面的dispatch_cb_）
    void DeleteConn(Connection *conn);   // 如何删除连接(把cfd从epoll中删除，然后关闭)，用于设置Connection里的delete_conn_cb_
    std::function<void(Connection *)> process_conn_cb_;  // 设置连接的行为，设置Connection里的process_cb_

private:    
    std::string     ip_;     // 服务器ip
    uint16_t        port_;   // 服务器port
    bool            is_stop_{false};  // 服务器是否关闭 

private:
    Reactor                  *main_reactor_{nullptr}; 
    Acceptor                 *acceptor_{nullptr};
    std::vector<Reactor*>    sub_reactors_;
    std::unordered_map<int, Connection *> conns_;


};







