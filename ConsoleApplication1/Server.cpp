#include "Server.h"

#include "Messages.h"
#include "RequestHandler.h"

namespace asio = boost::asio;
using asio::ip::tcp;

Server::~Server() {
    stop();
    thread_pool_.reset();
}


void Server::start() {
    log_->info("[Server::start] server is launching");
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

    log_->info("[Server::stop] server has been stopped");
}

void Server::runAcceptorLoop() {
    log_->info("[Server::runAcceptorLoop] start acceptor thread");

    while (!stop_requested_) {
        try {
            std::shared_ptr<tcp::socket> client_socket = std::make_shared<tcp::socket>(io_service_);
            acceptor_.accept(*client_socket);

            log_->info("[Server::runAcceptorLoop] accepted new client socket");

            RequestHandler handler = RequestHandler(client_socket, cache_, log_);
            thread_pool_->addTask(std::move(handler));
        }
        catch (const boost::system::system_error& e) {
            log_->error(std::string("[Server::runAcceptorLoop] Boost.Asio error: ") + e.what());
        }
        catch (const std::exception& e) {
            log_->error(std::string("[Server::runAcceptorLoop] exception in RunAcceptorLoop: ") + e.what());
        }
    }

    log_->info("[Server::runAcceptorLoop] acceptor thread has been stopped");
}