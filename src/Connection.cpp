#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "util.h"
#include "Buffer.h"
#include "EventLoop.h"
#include "Epoll.h"
#include "Server.h"
#include "Log.h"
#include "Timer.h"

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>




Connection::Connection(EventLoop *loop, int cfd) : loop_(loop), cfd_(cfd) {
    // 定义行为
    channel_ = new Channel(cfd_, loop_);
    channel_->SetEvents(EPOLLIN | EPOLLPRI);
    channel_->SetET();
    channel_->SetReadCallback([this] () {process_cb_(this);});
    channel_->UpdateChannelToReactor();

    // 新建连接时将这个连接加入到定时器中去
    loop_->GetHeapTimer()->Add(cfd_, loop_->timeout_MS_, std::bind(&Connection::Close, this));

    read_buf_  = new Buffer;
    write_buf_ = new Buffer;    

    state_ = State::Running;
}


Connection::~Connection(){
    if (channel_) delete channel_;
    if (read_buf_) delete read_buf_;
    if (write_buf_) delete write_buf_;
}



void 
Connection::Read() {
    ErrorIf(state_ == State::Closed, "connection has been disconnected");
    read_buf_->Clear();
    
    loop_->GetHeapTimer()->adjust(cfd_, loop_->timeout_MS_);  // 发生读事件了就加时长
    if (IsNonBlocking(cfd_)) {
        ReadNonBlocking();
    }else {
        ReadBlocking();
    }
}


void
Connection::Write() {
    ErrorIf(state_ == State::Closed, "Connection has been disconnected");

    loop_->GetHeapTimer()->adjust(cfd_, loop_->timeout_MS_);  // 发生写事件了就加时长

    if (IsNonBlocking(cfd_)) {
        WriteNonBlocking();
    }else {
        WriteBlocking();
    }
    write_buf_->Clear();
}

void
Connection::ReadNonBlocking() {
    char buf[1024];
    while (true) {  // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(cfd_, buf, sizeof(buf));
        if (bytes_read > 0) {
            read_buf_->Append(buf, bytes_read);
        } else if(bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
            std::cout << "continue read...\n";
            continue;
        } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {  // 非阻塞IO，这个条件表示数据全部读取完毕
            break;
        } else if (bytes_read == 0) {  // EOF，客户端断开连接
            std::cout << "read EOF, client fd " << cfd_ << " disconnected\n";
            // state_ = State::Closed;
            break;
        } else {
            std::cout << "Other error on the client fd " << cfd_ << ".\n";
            // state_ = State::Closed;
            break;
        }
    }
}


void
Connection::ReadBlocking() {
    unsigned int rcv_size = 0;
    socklen_t len = sizeof(rcv_size);
    getsockopt(cfd_, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
    char buf[rcv_size];
    ssize_t bytes_read = read(cfd_, buf, sizeof(buf));
    if (bytes_read > 0) {
        read_buf_->Append(buf, bytes_read);
    } else if (bytes_read == 0) {
        printf("read EOF, blocking client fd %d disconnected\n", cfd_);
        // state_ = State::Closed;
    } else if (bytes_read == -1) {
        printf("Other error on blocking client fd %d\n", cfd_);
        // state_ = State::Closed;
    }

}

void
Connection::WriteNonBlocking() {
    char buf[write_buf_->Size()];
    memcpy(buf, write_buf_->ToStr(), write_buf_->Size());
    int data_size = write_buf_->Size();
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t bytes_write = write(cfd_, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EINTR) {
            printf("continue writing\n");
            continue;
        }
        if (bytes_write == -1 && errno == EAGAIN) {
            break;
        }
        if (bytes_write == -1) {
            printf("Other error on client fd %d\n", cfd_);
            // state_ = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
}

void 
Connection::WriteBlocking() {
    // 没有处理send_buffer_数据大于TCP写缓冲区，的情况，可能会有bug
    ssize_t bytes_write = write(cfd_, write_buf_->ToStr(), write_buf_->Size());
    if (bytes_write == -1) {
        printf("Other error on blocking client fd %d\n", cfd_);
        // state_ = State::Closed;
    }
}

void
Connection::Close() {
    delete_conn_cb_(this);
}


Channel *
Connection::GetChannel() {
    return channel_;
}


Connection::State
Connection::GetState() {
    return state_;
}


int Connection::GetFd() {
    return cfd_;
}

EventLoop *
Connection::GetEventLoop() {
    return loop_;
}



void 
Connection::SetWriteBuf(const char *msg) {
    write_buf_->SetBuf(msg);
}

Buffer *
Connection::GetReadBuf() {
    return read_buf_;
}

Buffer *
Connection::GetWriteBuf() {
    return write_buf_;
}

void Connection::SetProcessCallback(std::function<void(Connection *)> const &callback) {
    process_cb_ = callback;
}


void Connection::SetDeleteConnCallback(std::function<void(Connection *)> const &callback) {
    delete_conn_cb_ = callback;
}

