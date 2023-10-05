#include "ThreadPool.h"

ThreadPool::~ThreadPool() {
    stopWorkers();
}

void ThreadPool::startWorkers() {
    for (int i = 0; i < threads_count_; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(mu_);
                    cond_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

                    if (stop_ && tasks_.empty()) {
                        return;
                    }

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                try {
                    task();
                }
                catch (std::exception& e) {
                    std::cout << "worker has exception during performing task: " << e.what() << std::endl;
                }
            }
            }
        );
    }
}

void ThreadPool::stopWorkers() {
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


