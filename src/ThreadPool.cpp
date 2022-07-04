#include "ThreadPool.h"




ThreadPool::ThreadPool(unsigned int size) {
    for (int i = 0; i < size; ++i) {
        auto th = std::thread(std::bind(&ThreadPool::Working, this));
        m_threads_.emplace_back(std::move(th));
    }
}

void
ThreadPool::Working(ThreadPool *pool) {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> locker(pool->m_mutex_);
            pool->m_condition_variable_.wait(locker, [pool](){return !(!pool->m_stop_ && pool->m_tasks_.empty());});
            if (pool->m_stop_ && pool->m_tasks_.empty()) return;
            task = pool->m_tasks_.front();
            pool->m_tasks_.pop();
        }
        task();
    }
}




ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> locker(m_mutex_);
        m_stop_ = true;
    }
    m_condition_variable_.notify_one();
    for (auto &th : m_threads_) {
        if (th.joinable()) th.join();
    }
}


void
ThreadPool::Add(const std::function<void()> &_task) {
    {
        std::unique_lock<std::mutex> locker(m_mutex_);
        if (m_stop_) {
            throw std::runtime_error("ThreadPool already stop, do not add the task any more");
        }
        m_tasks_.emplace(std::move(_task));
    }
    m_condition_variable_.notify_one();
}


