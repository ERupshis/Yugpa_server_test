#include "RequestHandler.h"

#include <iostream>
#include <filesystem>

#include "Encoder.h"
#include "ServerErrors.h"

void RequestHandler::operator()() {
    std::stringstream response_stream;
    messages::Response resp_msg;

    std::cout << "start handling request in thread: " << std::this_thread::get_id() << std::endl;

    try {
        if (!client_socket_->is_open()) {
            std::cerr << "Socket is not open." << std::endl;
            return;
        }

        std::stringstream request_stream;
        int32_t status = getRequest(request_stream);

        if (status != getErrorCode(ServerError::OK)) {
            resp_msg.status = status;
        } 
        else {
            messages::Request req_msg;
            Encoder::Deserialize(req_msg, request_stream);

            std::cout << "start handling request in thread: " << std::this_thread::get_id() << std::endl;

            resp_msg = getResponse(req_msg);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception in RequestHandler: " << e.what() << std::endl;
        resp_msg.status = getErrorCode(ServerError::INTERNAL_ERROR);
    }

    Encoder::Serialize(resp_msg, response_stream);
    sendResponse(response_stream);

    std::cout << "request handling in thread: " << std::this_thread::get_id() << " finished with status: " << resp_msg.status << std::endl;
}

int32_t RequestHandler::getRequest(std::stringstream& req_stream) {
    std::vector<char> buffer(1024);
    boost::system::error_code error;
    std::size_t bytes_read = client_socket_->read_some(boost::asio::buffer(buffer), error);

    if (error) {
        std::cerr << "Error reading from socket: " << error.message() << std::endl;
        return getErrorCode(ServerError::INTERNAL_ERROR);
    }

    if (bytes_read == 0) {
        return getErrorCode(ServerError::BAD_REQUEST);
    }

    for (size_t i = 0; i < bytes_read; i++) {
        req_stream << buffer[i];
    }

    return  getErrorCode(ServerError::OK);
}

messages::Response RequestHandler::getResponse(messages::Request& req) {
    messages::Response resp;
    std::filesystem::path dir_path(req.path.c_str());

    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        resp.directory_entries[entry.path().filename().string()] = entry.is_directory()
            ? messages::EntryType::DIRECTORY 
            : messages::EntryType::FILE;
    }

    resp.status = resp.directory_entries.empty()
        ? getErrorCode(ServerError::BAD_REQUEST)
        : getErrorCode(ServerError::OK);

    return resp;
}

void RequestHandler::sendResponse(std::stringstream& req_stream) {
    std::size_t bytes_read = client_socket_->send(boost::asio::buffer(req_stream.str()));

    if (!bytes_read) {
        std::cerr << "Error sending response in socket" << std::endl;
    }
}