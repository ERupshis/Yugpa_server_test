#pragma once

#include <boost/asio.hpp>

#include "Cache.h"
#include "Config.h"
#include "Log.h"
#include "ThreadPool.h"

class Server {
public:
    Server(const Config& cfg, std::shared_ptr<Log> log)
        :threads_num_(cfg.threads_num), 
        acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), cfg.port)),
        thread_pool_(std::make_unique<ThreadPool>(threads_num_, log)),
        cache_(std::make_shared<Cache>(cfg.cache_interval, log)),
        log_(log)
    {}

    ~Server();

    void start();
    void stop();

private:
    void runAcceptorLoop();

private:
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    bool stop_requested_ = false;
    int threads_num_ = 1;

    std::thread accept_thread_;
    std::unique_ptr<ThreadPool> thread_pool_ = nullptr;
    std::shared_ptr<Cache> cache_ = nullptr;

    std::shared_ptr<Log> log_ = nullptr;
};
