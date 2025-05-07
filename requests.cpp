#include "requests.h"
#include <sstream>

std::string compute_get_request(const std::string& host,
                                const std::string& path,
                                const std::string& query_params,
                                const std::vector<std::string>& cookies) {
    std::stringstream message;

    if (!query_params.empty()) {
        message << "GET " << path << "?" << query_params << " HTTP/1.1\r\n";
    } else {
        message << "GET " << path << " HTTP/1.1\r\n";
    }

    message << "Host: " << host << "\r\n";

    message << "Content-Type: application/x-www-form-urlencoded\r\n";

    if (!cookies.empty()) {
        message << "Cookie: " << cookies[0];
        for (size_t i = 1; i < cookies.size(); ++i) {
            message << "; " << cookies[i];
        }
        message << "\r\n";
    }

    message << "\r\n";

    return message.str();
}

std::string compute_post_request(const std::string& host,
                                 const std::string& path,
                                 const std::string& content_type,
                                 const std::vector<std::string>& body_data,
                                 const std::vector<std::string>& cookies) {
    std::stringstream message;

    message << "POST " << path << " HTTP/1.1\r\n";

    message << "Host: " << host << "\r\n";

    if (!content_type.empty()) {
        message << "Content-Type: " << content_type << "\r\n";
    }

    std::string body_data_buffer;
    if (!body_data.empty()) {
        body_data_buffer = body_data[0];
        for (size_t i = 1; i < body_data.size(); ++i) {
            body_data_buffer += "&" + body_data[i];
        }
    }

    message << "Content-Length: " << body_data_buffer.length() << "\r\n";

    if (!cookies.empty()) {
        for (const auto& cookie : cookies) {
            message << "Cookie: " << cookie << "\r\n";
        }
    }

    message << "\r\n";

    message << body_data_buffer;

    return message.str();
}
