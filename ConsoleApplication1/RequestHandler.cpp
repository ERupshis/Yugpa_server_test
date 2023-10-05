#include "RequestHandler.h"

#include <iostream>
#include <filesystem>
#include <chrono>

#include "Encoder.h"
#include "ServerErrors.h"
#include "ThreadPool.h"

using timestamp = std::chrono::system_clock::time_point;


void RequestHandler::operator()() {
    messages::Request req_msg;

    std::stringstream response_stream;
    messages::Response resp_msg;

    log_->info("[RequestHandler::operator()] start handling request in thread: ",  std::this_thread::get_id());

    try {
        if (!client_socket_->is_open()) {
            log_->info("[RequestHandler::operator()] socket is not open in thread: ", std::this_thread::get_id());
            return;
        }

        std::stringstream request_stream;
        int32_t status = getRequest(request_stream);

        if (status != getErrorCode(ServerError::OK)) {
            resp_msg.status = status;
        } 
        else {
            Encoder::Deserialize(req_msg, request_stream);

            std::function<messages::Response()> handleRequest = [&](){
                return getResponse(req_msg);
                };

            resp_msg = cache_->getValueAndRefreshIfNeed(req_msg.path, handleRequest);
        }
    }
    catch (std::exception& e) {
        log_->error("[RequestHandler::operator()] exception caught '", e.what(), "' in thread: ", std::this_thread::get_id());
        resp_msg.status = getErrorCode(ServerError::INTERNAL_ERROR);
    }

    Encoder::Serialize(resp_msg, response_stream);
    sendResponse(response_stream);
    client_socket_->close();

    log_->info("[RequestHandler::operator()] request handling in thread: ", std::this_thread::get_id(), " finished with status: ", resp_msg.status);
}

int32_t RequestHandler::getRequest(std::stringstream& req_stream) {
    log_->info("[RequestHandler::getRequest] extracting request from message in thread: ", std::this_thread::get_id());

    std::vector<char> buffer(1024);
    boost::system::error_code error;
    std::size_t bytes_read = client_socket_->read_some(boost::asio::buffer(buffer), error);

    if (error) {
        log_->info("[RequestHandler::getRequest] error reading from socket: '", error.message(), "' in thread : ", std::this_thread::get_id());
        return getErrorCode(ServerError::INTERNAL_ERROR);
    }

    if (bytes_read == 0) {
        log_->info("[RequestHandler::getRequest] empty request from socket in thread : ", std::this_thread::get_id());
        return getErrorCode(ServerError::BAD_REQUEST);
    }

    for (size_t i = 0; i < bytes_read; i++) {
        req_stream << buffer[i];
    }

    return  getErrorCode(ServerError::OK);
}

messages::Response RequestHandler::getResponse(messages::Request& req) {
    log_->info("[RequestHandler::getResponse] parsing directory's entries '", req.path ,"' in thread : ", std::this_thread::get_id());

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
    log_->info("[RequestHandler::sendResponse] sending result to client in thread : ", std::this_thread::get_id());
    std::size_t bytes_read = client_socket_->send(boost::asio::buffer(req_stream.str()));

    if (!bytes_read) {
        log_->info("[RequestHandler::sendResponse] couldn't sending response to client in thread : ", std::this_thread::get_id());
    }
}