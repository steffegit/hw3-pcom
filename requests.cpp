#include "requests.h"
#include <sstream>
#include "nlohmann.hpp"

using json = nlohmann::json;

std::string compute_get_request(const std::string& host,
                                const std::string& path,
                                const json& body_data,
                                const std::vector<std::string>& cookies) {
    std::stringstream message;

    message << "GET " << path << " HTTP/1.1\r\n";
    message << "Host: " << host << "\r\n";
    message << "Content-Type: application/json\r\n";

    std::string body_data_buffer;
    body_data_buffer = body_data.dump();
    message << "Content-Length: " << body_data_buffer.length() << "\r\n";

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "\r\n";
    message << body_data_buffer;

    return message.str();
}

std::string compute_post_request(const std::string& host,
                                 const std::string& path,
                                 const json& body_data,
                                 const std::vector<std::string>& cookies,
                                 const std::string& authorization) {
    std::stringstream message;

    message << "POST " << path << " HTTP/1.1\r\n";
    message << "Host: " << host << "\r\n";
    message << "Content-Type: application/json\r\n";

    std::string body_data_buffer;
    body_data_buffer = body_data.dump();

    message << "Content-Length: " << body_data_buffer.length() << "\r\n";

    if (!authorization.empty()) {
        message << "Authorization: Bearer " << authorization << "\r\n";
    }

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "\r\n";
    message << body_data_buffer;

    return message.str();
}
