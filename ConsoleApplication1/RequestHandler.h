#pragma once

#include <boost/asio.hpp>

#include "Cache.h"
#include "Log.h"
#include "Messages.h"

class RequestHandler {
public:
    RequestHandler(std::shared_ptr<boost::asio::ip::tcp::socket> client_socket, std::shared_ptr<Cache> cache, std::shared_ptr<Log> log)
        :client_socket_(client_socket),
        cache_(cache),
        log_(log)
    {}

    ~RequestHandler() = default;

    void operator()();

private:
    int32_t getRequest(std::stringstream& req_stream);
    messages::Response getResponse(messages::Request& req);

    void sendResponse(std::stringstream& req_stream);

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> client_socket_ = nullptr;
    std::shared_ptr<Cache> cache_ = nullptr;

    std::shared_ptr<Log> log_ = nullptr;
};