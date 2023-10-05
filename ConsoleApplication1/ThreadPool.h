#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t threads_count)
        :threads_count_(threads_count)
    {}

    ~ThreadPool();

    void startWorkers();
    void stopWorkers();

    template <typename Func>
    void addTask(Func&& f);


private:
    int threads_count_ = 1;
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex mu_;
    std::condition_variable cond_;
    bool stop_ = false;
};

template <typename Func>
void ThreadPool::addTask(Func&& f) {
    {
        std::unique_lock<std::mutex> lock(mu_);
        tasks_.emplace(std::forward<Func>(f));
    }
    cond_.notify_one();
}