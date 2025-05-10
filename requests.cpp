#include "requests.h"
#include <sstream>
#include "nlohmann.hpp"

using json = nlohmann::json;

std::string compute_get_request(const std::string& host,
                                const std::string& path,
                                const json& body_data,
                                const std::vector<std::string>& cookies) {
    std::stringstream message;
    std::string body_data_buffer;

    message << "GET " << path << " HTTP/1.1\r\n";
    message << "Host: " << host << "\r\n";
    // message << "Connection: keep-alive\r\n";
    message << "Content-Type: application/json\r\n";

    if (!body_data.empty()) {
        body_data_buffer = body_data.dump();
        message << "Content-Length: " << body_data_buffer.length() << "\r\n";
    }

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "\r\n";

    if (!body_data.empty()) {
        message << body_data_buffer;
    }

    return message.str();
}

std::string compute_get_request(const std::string& host,
                                const std::string& path,
                                const json& body_data,
                                const std::vector<std::string>& cookies,
                                const std::string& jwt_token) {
    std::stringstream message;
    std::string body_data_buffer;

    message << "GET " << path << " HTTP/1.1\r\n";
    message << "Host: " << host << "\r\n";
    // message << "Connection: keep-alive\r\n";
    message << "Content-Type: application/json\r\n";

    if (!body_data.empty()) {
        body_data_buffer = body_data.dump();
        message << "Content-Length: " << body_data_buffer.length() << "\r\n";
    }

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "Authorization: Bearer " << jwt_token << "\r\n";

    message << "\r\n";

    if (!body_data.empty()) {
        message << body_data_buffer;
    }

    return message.str();
}

std::string compute_post_request(const std::string& host,
                                 const std::string& path,
                                 const json& body_data,
                                 const std::vector<std::string>& cookies) {
    std::stringstream message;
    std::string body_data_buffer;

    message << "POST " << path << " HTTP/1.1\r\n";
    message << "Host: " << host << "\r\n";
    // message << "Connection: keep-alive\r\n";
    message << "Content-Type: application/json\r\n";

    if (!body_data.empty()) {
        body_data_buffer = body_data.dump();
        message << "Content-Length: " << body_data_buffer.length() << "\r\n";
    }

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "\r\n";

    if (!body_data.empty()) {
        message << body_data_buffer;
    }

    return message.str();
}

std::string compute_post_request(const std::string& host,
                                 const std::string& path,
                                 const json& body_data,
                                 const std::vector<std::string>& cookies,
                                 const std::string& jwt_token) {
    std::stringstream message;
    std::string body_data_buffer;

    message << "POST " << path << " HTTP/1.1\r\n";
    message << "Host: " << host << "\r\n";
    // message << "Connection: keep-alive\r\n";
    message << "Content-Type: application/json\r\n";

    if (!body_data.empty()) {
        body_data_buffer = body_data.dump();
        message << "Content-Length: " << body_data_buffer.length() << "\r\n";
    }

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "Authorization: Bearer " << jwt_token << "\r\n";

    message << "\r\n";

    if (!body_data.empty()) {
        message << body_data_buffer;
    }

    return message.str();
}

std::string compute_delete_request(const std::string& host,
                                   const std::string& path,
                                   const std::vector<std::string>& cookies) {
    std::stringstream message;

    message << "DELETE " << path << " HTTP/1.1\r\n";
    message << "Host: " << host << "\r\n";
    message << "Accept: application/json\r\n";

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "\r\n";  // End of headers

    return message.str();
}
