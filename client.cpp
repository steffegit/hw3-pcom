#include <sys/socket.h>
#include <iostream>
#include <string>
#include "helpers.h"
#include "nlohmann.hpp"
#include "requests.h"

using json = nlohmann::json;

std::string login_admin(int sockfd, std::string host) {
    std::string username, password;
    std::cin >> username >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/admin/login", body_data, {});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd);
    std::cout << response << std::endl;

    std::string cookie =
        response.substr(response.find("Set-Cookie: ") + 13, response.find(";"));
    return cookie;
}

void add_user(int sockfd, std::string host, std::string cookie) {
    std::string username, password;
    std::cin >> username >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request = compute_post_request(host, "/api/v1/tema/admin/users",
                                               body_data, {cookie});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd);
    std::cout << response << std::endl;
}

int main() {
    std::string HOST = "63.32.125.183";
    int PORT = 8081;

    std::string hostname = HOST + ":" + std::to_string(PORT);

    int sockfd = open_conn(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    std::string cookie = login_admin(sockfd, hostname);
    std::cout << cookie << std::endl;
}