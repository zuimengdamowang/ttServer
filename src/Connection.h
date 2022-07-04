#pragma once
#include <functional>

class EventLoop;
class Socket;
class Channel;
class Buffer;

// Connection可以视为对通信描述符的一种封装，并定义了有关行为
// 注意：一个Connection需要知道自己属于哪一个sub-reactor


class Connection {
public:
    enum State {
        Closed = 0, Running,
    };

public:    
    Connection(EventLoop *loop, int cfd);
    ~Connection();

public:
    void Read();    // 将数据读到读缓存
    void Write();   // 将数据写入写缓存
    void Close();   // 关闭连接

    Channel*    GetChannel();
    State       GetState();
    int         GetFd();
    EventLoop*  GetEventLoop();
    Buffer*     GetReadBuf();
    Buffer*     GetWriteBuf();

    void SetWriteBuf(const char *msg);
    void SetProcessCallback(std::function<void(Connection *)> const &callback);
    void SetDeleteConnCallback(std::function<void(Connection *)> const &callback);

private:
    // 以下是具体读写的方式
    void ReadNonBlocking(); 
    void WriteNonBlocking();
    void ReadBlocking();
    void WriteBlocking();

private:
    EventLoop *loop_{nullptr};       // 一个连接需要知道自己属于那个sub-reactor 
    int       cfd_{-1};              // 这个连接对应的通信描述符

private:
    State     state_{State::Closed};        // 这个连接的状态
    Channel   *channel_{nullptr};           // 这个连接的行为
    Buffer    *read_buf_ {nullptr};         // 连接的读缓存
    Buffer    *write_buf_{nullptr};         // 连接的写缓存
    std::function<void(Connection *)> process_cb_;          // 这个连接的行为回调函数（这个回调函数决定了连接具体行为，是单纯的echo，还是http。。。）
    std::function<void(Connection *)> delete_conn_cb_;      // 删除连接
    
};
