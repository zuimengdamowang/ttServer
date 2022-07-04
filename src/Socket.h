#pragma once
#include <arpa/inet.h>

// TCP通信地址相关的类
// 内部主要封装了struct sockaddr_in 这个结构体
struct InetAddress{
    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress() = default;
    
    struct sockaddr_in m_addr_{};
};


class Socket{
public:
    Socket();
    Socket(int fd);
    ~Socket();

    void Bind(InetAddress *inetAddr);
    void Listen();
    int  Accept(InetAddress *clntInetAddr);
    void Connect(InetAddress *servInetAddr);
    
    int GetFd();
    

private:
    int m_fd_{-1};

};

