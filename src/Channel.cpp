#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Epoll.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>

Channel::Channel(int fd, EventLoop *loop) : loop_(loop), fd_(fd), is_in_epoll_(false) {}

Channel::~Channel(){
    if(fd_ != -1){
        close(fd_);
        fd_ = -1;
    }
}

void Channel::HandleEvent(){
    if(ready_events_ & (EPOLLIN | EPOLLPRI)){
        read_cb_();
    }
    if(ready_events_ & EPOLLOUT){
        write_cb_();
    }

}

int Channel::GetFd(){
    return fd_;
}

uint32_t Channel::GetEvents(){
    return events_;
}

uint32_t Channel::GetReadyEvents(){
    return ready_events_;
}

bool Channel::GetIsInEpoll() {
    return is_in_epoll_;
}


void Channel::SetIsInEpoll(bool cond) {
    is_in_epoll_ = cond;
}

void Channel::SetReadCallback(std::function<void()> const &callback){
    read_cb_ = callback;
}

void Channel::SetWriteCallback(std::function<void()> const &callback){
    write_cb_ = callback;
}


void Channel::SetEvents(uint32_t events) {
    events_ |= events;
}

void Channel::SetReadyEvents(uint32_t events){
    ready_events_ |= events;
}

void Channel::SetET(){
    events_ |= EPOLLET;
}


void Channel::UpdateChannelToReactor() {
    loop_->UpdateChannelToEpoll(this);
}


