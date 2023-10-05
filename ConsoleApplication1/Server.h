#pragma once

#include <boost/asio.hpp>

#include "ThreadPool.h"

class Server {
public:
    Server(int port, int threads_num)
        :threads_num_(threads_num), 
        acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        thread_pool_(std::make_unique<ThreadPool>(threads_num_))
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
};
