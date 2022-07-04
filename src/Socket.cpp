#include "Socket.h"
#include <cstring>
#include "util.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

InetAddress::InetAddress() = default;

InetAddress::InetAddress(const char *ip, uint16_t port) {
    memset(&m_addr_, 0, sizeof(m_addr_));
    m_addr_.sin_family = AF_INET;
    m_addr_.sin_port = htons(port);
    m_addr_.sin_addr.s_addr = inet_addr(ip);
}




Socket::Socket() {
    m_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrorIf(m_fd_ == -1, "socket create error");
}

Socket::Socket(int fd): m_fd_(fd) {
    ErrorIf(m_fd_ == -1, "socket create error");
}

Socket::~Socket() {
    if (-1 != m_fd_) {
        close(m_fd_);
        m_fd_ = -1;
    }
}

void
Socket::Bind(InetAddress *inetAddr) {
    int flag = bind(m_fd_, (sockaddr *)&inetAddr->m_addr_, sizeof(struct sockaddr_in));
    ErrorIf(flag == -1, "socket bind error");
}


void
Socket::Listen() {
    int flag = listen(m_fd_, SOMAXCONN);
    ErrorIf(-1 == flag, "socket listen error");
}




// only for server
int
Socket::Accept(InetAddress *clntInetAddr) {
    int cfd = -1;
    struct sockaddr_in tmp_addr{};
    socklen_t len = sizeof(struct sockaddr_in);
    if (IsNonBlocking(m_fd_)) {
        while (true) {
            cfd = accept(m_fd_, (sockaddr *)&tmp_addr, &len);
            if (cfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                // printf("no connection yet\n");
                continue;
            }
            if (cfd == -1) {
                ErrorIf(true, "socket accept error");
            } else {
                break;
            }
        }   
    }else {
        cfd = accept(m_fd_, (sockaddr *)&tmp_addr, &len);
        ErrorIf(-1 == cfd, "socket accept error");
    }


    // if (!clntInetAddr) {
    //     cfd = ::accept(m_fd_, nullptr, nullptr);
    // }else {
    //     cfd = ::accept(m_fd_, (sockaddr *)&clntInetAddr->m_addr_, &len);
    // }
    // // std::cout << cfd << std::endl;
    // ErrorIf(-1 == cfd, "socket accept error");

    return cfd;
}

// only for client
void 
Socket::Connect(InetAddress *servInetAddr) {
    if (IsNonBlocking(m_fd_)) {
         while (true) {
            int ret = connect(m_fd_, (sockaddr *)&servInetAddr->m_addr_, sizeof(struct sockaddr_in));
            if (ret == 0) {
                break;
            }
            if (ret == -1 && (errno == EINPROGRESS)) {
                continue;
            }
            if (ret == -1) {
                ErrorIf(true, "socket connect error");
            }
        }
    }else {
        int flag = connect(m_fd_, (sockaddr *)&servInetAddr->m_addr_, sizeof(struct sockaddr_in));
        ErrorIf(-1 == flag, "socket connect error");
    }
}


int
Socket::GetFd() {
    return m_fd_;
}









