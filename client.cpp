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

std::string login_admin(int sockfd, std::string host) {
    std::string username, password;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request = compute_post_request(host, "/api/v1/tema/admin/login",
                                               body_data, {}, "");

    send_request(sockfd, request);
    std::string response = recv_response(sockfd);

    // std::cout << response << std::endl;  // DEBUG

    if (response.find("200") != std::string::npos) {
        success_msg("Admin autentificat cu succes");
    } else {
        error_msg("Nu am putut autentifica adminul");
    }

    size_t cookie_start =
        response.find("Set-Cookie: ") + 12;  // +12 to skip "Set-Cookie: "
    size_t cookie_end = response.find(";", cookie_start);
    std::string cookie =
        response.substr(cookie_start, cookie_end - cookie_start);

    cookie = cookie.substr(cookie.find("=") + 1);  // strip of "session="

    return cookie;
}

void add_user(int sockfd, std::string host, std::string JWT_token) {
    std::string username, password;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request = compute_post_request(host, "/api/v1/tema/admin/users",
                                               body_data, {}, JWT_token);

    std::cout << request << std::endl;  // DEBUG

    send_request(sockfd, request);
    // FIXME: cred ca e de la recv_response, pare ca daca da recv o data, nu mai
    // poate citi dupa
    std::string response = recv_response(sockfd);

    std::cout << response << std::endl;  // DEBUG

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
    std::string jwt_cookie = login_admin(sockfd, host);
    add_user(sockfd, host, jwt_cookie);

    close_conn(sockfd);
    return 0;
}