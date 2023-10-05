#include "Server.h"

#include "Messages.h"
#include "RequestHandler.h"

namespace asio = boost::asio;
using asio::ip::tcp;

Server::~Server() {
    stop();
    thread_pool_.reset(nullptr);
}


void Server::start() {
    accept_thread_ = std::thread([this]() { runAcceptorLoop(); });
    thread_pool_->startWorkers();
}

void Server::stop() {
    stop_requested_ = true;
    acceptor_.close();

    io_service_.stop();

    if (accept_thread_.joinable()) {
        accept_thread_.join();
    }
}

void Server::runAcceptorLoop() {
    while (!stop_requested_) {
        try {
            std::shared_ptr<tcp::socket> client_socket = std::make_shared<tcp::socket>(io_service_);
            acceptor_.accept(*client_socket);
            RequestHandler handler = RequestHandler(client_socket);
            thread_pool_->addTask(std::move(handler));
        }
        catch (const boost::system::system_error& e) {
            std::cerr << "Boost.Asio error: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in RunAcceptorLoop: " << e.what() << std::endl;
        }
    }
}