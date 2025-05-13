#include "helpers.h"

void error(const std::string& msg) {
    perror(msg.c_str());
    exit(1);
}

void success_msg(std::string msg) {
    std::cout << "SUCCESS: " << msg << std::endl;
}

void error_msg(std::string msg) {
    std::cout << "ERROR: " << msg << std::endl;
}

bool status_code(std::string response, int expected_code) {
    std::string line = response.substr(0, response.find("\r\n"));  // first line
    return line.find(std::to_string(expected_code)) != std::string::npos;
}

int open_conn(std::string host,
              int port,
              int iptype,
              int socket_type,
              int flags) {
    int sockfd = socket(iptype, socket_type, flags);
    if (sockfd == -1)
        error("Error creating socket");

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(host.c_str());

    if (connect(sockfd, (struct sockaddr*)&server_address,
                sizeof(server_address)) == -1)
        error("Error connecting to server");

    return sockfd;
}

void close_conn(int sockfd) {
    close(sockfd);
}

void send_request(int& sockfd, std::string host, std::string message) {
    size_t sent = 0;
    const size_t total = message.length();

    while (sent < total) {
        ssize_t bytes = write(sockfd, message.data() + sent, total - sent);

        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += static_cast<size_t>(bytes);
    }
}

std::string recv_response(int& sockfd, std::string host) {
    std::string buffer;
    buffer.reserve(BUFLEN);
    std::vector<char> response(BUFLEN);
    size_t header_end = 0;
    size_t content_length = 0;

    do {
        ssize_t bytes = read(sockfd, response.data(), BUFLEN);
        if (bytes < 0) {
            error("ERROR reading response from socket");
        }
        if (bytes == 0) {
            break;
        }

        buffer.append(response.data(), static_cast<size_t>(bytes));

        header_end = buffer.find(HEADER_TERMINATOR);
        if (header_end != std::string::npos) {
            header_end += HEADER_TERMINATOR_SIZE;

            size_t content_length_start = buffer.find(CONTENT_LENGTH);
            if (content_length_start != std::string::npos) {
                content_length_start += CONTENT_LENGTH_SIZE;
                content_length =
                    std::stoul(buffer.substr(content_length_start));
                break;
            } else {
                break;
            }
        }
    } while (true);

    if (content_length > 0) {
        size_t total = content_length + header_end;
        while (buffer.length() < total) {
            ssize_t bytes = read(sockfd, response.data(), BUFLEN);
            if (bytes <= 0) {
                break;
            }
            buffer.append(response.data(), static_cast<size_t>(bytes));
        }
    }

    return buffer;
}