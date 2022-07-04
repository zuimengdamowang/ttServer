#include "Epoll.h"
#include "util.h"
#include "Channel.h"
#include <unistd.h>
#include <cstring>
#include <iostream>

#define MAX_EVENTS 1024


Epoll::Epoll(): events_(MAX_EVENTS) {
    epfd_ = epoll_create1(0);
    ErrorIf(epfd_ == -1, "epoll create error");
}

Epoll::~Epoll(){
    if(epfd_ != -1){
        close(epfd_);
        epfd_ = -1;
    }
}

std::vector<Channel*> 
Epoll::Poll(int timeout){
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd_, &events_[0], MAX_EVENTS, timeout);
    ErrorIf(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++i){
        Channel *ch = (Channel*)events_[i].data.ptr;
        ch->SetReadyEvents(events_[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}


void Epoll::UpdateChannel(Channel *channel) {
    int fd = channel->GetFd();
    struct epoll_event ev{};
    ev.data.ptr = (void *)channel;
    ev.events   = channel->GetEvents();
    if (channel->GetIsInEpoll() == true) {  // mod
        int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev);
        ErrorIf(-1 == ret, "epoll mod error");
    } else {  // add
        int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
        ErrorIf(-1 == ret, "epoll add error");
        channel->SetIsInEpoll(true);
    }
    
}


void Epoll::DelChannel(Channel *channel){
    int fd = channel->GetFd();
    ErrorIf(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1, "epoll delete error");
    channel->SetIsInEpoll(false);
}