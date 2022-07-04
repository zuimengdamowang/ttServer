#pragma once
#include <queue>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>


class ThreadPool {
public:
    ThreadPool(unsigned int size = std::thread::hardware_concurrency());
    ~ThreadPool();

    void Add(const std::function<void()> &_task);
    // template <class F, class... Args> auto Add(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    static void Working(ThreadPool *pool);

private:
    std::queue<std::function<void()>> m_tasks_;
    std::vector<std::thread>          m_threads_;
    std::mutex                        m_mutex_;
    std::condition_variable           m_condition_variable_;

private:
    bool m_stop_{false};
    
    
};



// template <class F, class... Args>
// auto ThreadPool::Add(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
//     using return_type = typename std::result_of<F(Args...)>::type;

//     auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

//     std::future<return_type> res = task->get_future();
//     {
//         std::unique_lock<std::mutex> lock(m_mutex_);

//         // don't allow enqueueing after stopping the pool
//         if (m_stop_) {
//             throw std::runtime_error("enqueue on stopped ThreadPool");
//         }

//         m_tasks_.emplace([task]() { (*task)(); });
//     }
//     m_condition_variable_.notify_one();
//     return res;
// }


