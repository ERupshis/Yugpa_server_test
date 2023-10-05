#include "ThreadPool.h"

#include <sstream>

ThreadPool::~ThreadPool() {
    stopWorkers();
}

void ThreadPool::startWorkers() {
    log_->info("[ThreadPool::startWorkers] starting work in '", threads_count_, "' threads");
    for (int i = 0; i < threads_count_; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(mu_);
                    cond_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

                    if (stop_ && tasks_.empty()) {
                        log_->info("[ThreadPool::startWorkers] thread '", std::this_thread::get_id(), "' is stopping");
                        return;
                    }


                    log_->info("[ThreadPool::startWorkers] thread '", std::this_thread::get_id(), "' is extracting task from queue");
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                task();
            }
            }
        );
    }
}

void ThreadPool::stopWorkers() {
    log_->info("[ThreadPool::stopWorkers] pool is finishing execution");
    {
        std::unique_lock<std::mutex> lock(mu_);
        stop_ = true;
    }
    cond_.notify_all();

    for (std::thread& worker : workers_) {
        worker.join();
    }

    workers_.clear();
}
