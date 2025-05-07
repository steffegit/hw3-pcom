#include <sys/socket.h>
#include <iostream>
#include <string>
#include "helpers.h"
#include "nlohmann.hpp"
#include "requests.h"

using json = nlohmann::json;

void success_msg(std::string msg) {
    std::cout << "SUCCESS: " << msg << std::endl;
}

void error_msg(std::string msg) {
    std::cout << "ERROR: " << msg << std::endl;
}

bool success(std::string response, int expected_code) {
    std::string line = response.substr(0, response.find("\r\n"));  // first line
    return line.find(std::to_string(expected_code)) != std::string::npos;
}

std::string login_admin(int sockfd, std::string host) {
    std::string username, password;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/admin/login", body_data, {});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd);

    if (success(response, 200)) {
        success_msg("Admin autentificat cu succes");
    } else {
        error_msg("Nu am putut autentifica adminul");
    }

    // session cookie
    size_t cookie_start =
        response.find("Set-Cookie: ") + 12;  // +12 to skip "Set-Cookie: "
    size_t cookie_end = response.find(";", cookie_start);
    std::string cookie =
        response.substr(cookie_start, cookie_end - cookie_start);

    if (cookie.empty()) {
        error_msg("Nu am putut obtine cookie-ul");
    }

    return cookie;
}

void add_user(int sockfd, std::string host, std::string session_cookie) {
    std::string username, password;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"username", username}, {"password", password}};
    std::string cookie = "session=" + session_cookie;

    std::string request = compute_post_request(host, "/api/v1/tema/admin/users",
                                               body_data, {cookie});

    std::cout << request << std::endl;

    send_request(sockfd, request);
    std::string response = recv_response(sockfd);

    // 201 = created
    // 209 = conflict

    if (response.find("201") != std::string::npos) {
        success_msg("User adaugat cu succes");
    } else {
        error_msg("Nu am putut adauga userul");
    }
}

int main() {
    std::string IP = "63.32.125.183";
    int PORT = 8081;

    std::string host = IP + ":" + std::to_string(PORT);

    int sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
    std::string session_cookie = login_admin(sockfd, host);

    if (!session_cookie.empty()) {
        // reopen connection
        close_conn(sockfd);
        sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
    }

    add_user(sockfd, host, session_cookie);

    close_conn(sockfd);
    return 0;
}