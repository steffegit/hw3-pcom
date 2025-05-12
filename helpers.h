#pragma once

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

constexpr size_t BUFLEN = 8096;
constexpr const char* HEADER_TERMINATOR = "\r\n\r\n";
constexpr size_t HEADER_TERMINATOR_SIZE = 4;
constexpr const char* CONTENT_LENGTH = "Content-Length: ";
constexpr size_t CONTENT_LENGTH_SIZE = 16;

void error(const std::string& msg);
void success_msg(std::string msg);
void error_msg(std::string msg);
bool status_code(std::string response, int expected_code);
int open_conn(std::string host,
              int port,
              int iptype,
              int socket_type,
              int flags);
void close_conn(int sockfd);
void send_request(int& sockfd, std::string host, std::string message);
std::string recv_response(int& sockfd, std::string host);
