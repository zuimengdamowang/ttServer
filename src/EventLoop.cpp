#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "Server.h"
#include "Timer.h"
#include "Log.h"
#include "ThreadPool.h"
#include "Timer.h"

#include <vector>
#include <iostream>



EventLoop::EventLoop(Server *server) : 
    server_(server), is_reactor_stop_(false)
{
    epoll_ = new Epoll;
    threadpool_ = new ThreadPool(std::thread::hardware_concurrency());
    timer_ = new HeapTimer;
}


EventLoop::~EventLoop() {
    delete epoll_;
}



void
EventLoop::Loop() {
    while (is_reactor_stop_ == false) {
        int timeout = timer_->GetNextTick();
        std::vector<Channel *> channels = epoll_->Poll(timeout);
        for (auto cha : channels) {
            threadpool_->Add(std::bind(&Channel::HandleEvent, cha));
            
        };
        
    }
}



Epoll *
EventLoop::GetEpoll() {
    return epoll_;
}


Server *
EventLoop::GetServer() {
    return server_;
}


HeapTimer*
EventLoop::GetHeapTimer() {
    return timer_;
}



void EventLoop::UpdateChannelToEpoll(Channel *cha) {
    epoll_->UpdateChannel(cha);
}

void EventLoop::DelChannelfromEpoll(Channel *cha) {
    epoll_->DelChannel(cha);
}

