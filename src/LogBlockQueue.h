#pragma once
#include <mutex>
#include <condition_variable>
#include <cassert>

#include <queue>
#include <list>

#include "util.h"



template<class T>
class BlockQueue {
public:
    BlockQueue(int size = 1024);
    ~BlockQueue();

public:
    void push(const T &item);
    void pop ();
    bool empty();
    bool full();
    T front();
    T back();


private:
    typename std::mutex m_mutex_empty_;
    std::condition_variable m_cv_empty_;
    std::mutex m_mutex_full_;
    std::condition_variable m_cv_full_;

private:
    std::queue<T, std::list<T>> m_queue_;
    int m_max_size_{0};
    int m_size_{0};

};


template<class T>
BlockQueue<T>::BlockQueue(int size) {
    ErrorIf (size <= 0, "size <= 0, error!");

    m_max_size_ = size;
}


template <class T>
BlockQueue<T>::~BlockQueue() = default;


template <class T>
void
BlockQueue<T>::push(const T &item) {
    std::unique_lock<std::mutex> locker(m_mutex_full_);
    m_cv_full_.wait(locker, [this](){
        // 不满返回true，满了返回false
        if (m_size_ < m_max_size_) {
            return true;
        }
        return false;
    });
    m_queue_.push(item);
    ++m_size_;
    m_cv_empty_.notify_one();
}


template <class T>
void 
BlockQueue<T>::pop() {
    std::unique_lock<std::mutex> locker(m_mutex_empty_);
    m_cv_empty_.wait(locker, [this](){
        if (m_size_ != 0) {
            return true;
        }
        return false;
    });
    m_queue_.pop();
    --m_size_;
    m_cv_full_.notify_one();
}


template <class T>
bool 
BlockQueue<T>::empty() {
    std::unique_lock<std::mutex> locker(m_mutex_empty_);
    assert(m_size_ == (int)m_queue_.size());
    return m_size_ == 0;
}

template <class T>
bool
BlockQueue<T>::full() {
    std::unique_lock<std::mutex> locker(m_mutex_full_);
    return m_queue_.size() == m_max_size_;
}


template <class T>
T
BlockQueue<T>::front() {
    std::unique_lock<std::mutex> locker(m_mutex_empty_);
    m_cv_empty_.wait(locker, [this](){
        if (m_size_ != 0) {
            return true;
        }
        return false;
    });
    return m_queue_.front();
}

template <class T>
T
BlockQueue<T>::back() {
    std::unique_lock<std::mutex> locker(m_mutex_empty_);
    m_cv_empty_.wait(locker, [this](){
        if (m_size_ != 0) {
            return true;
        }
        return false;
    });
    return m_queue_.back();
}
