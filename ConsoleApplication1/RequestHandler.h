#pragma once

#include <boost/asio.hpp>

#include "Messages.h"

class RequestHandler {
public:
    RequestHandler(std::shared_ptr<boost::asio::ip::tcp::socket> client_socket)
    :client_socket_(client_socket)
    {}

    void operator()();

private:
    int32_t getRequest(std::stringstream& req_stream);
    messages::Response getResponse(messages::Request& req);

    void sendResponse(std::stringstream& req_stream);

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> client_socket_;

};