#pragma once

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

constexpr size_t BUFLEN = 4096;
constexpr const char* HEADER_TERMINATOR = "\r\n\r\n";
constexpr size_t HEADER_TERMINATOR_SIZE = 4;
constexpr const char* CONTENT_LENGTH = "Content-Length: ";
constexpr size_t CONTENT_LENGTH_SIZE = 16;

void error(const std::string& msg);
int open_conn(std::string host,
              int port,
              int iptype,
              int socket_type,
              int flags);
void close_conn(int sockfd);
void send_to_server(int sockfd, const std::string& message);
std::string recv_from_server(int sockfd);
