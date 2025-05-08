#include <sys/socket.h>
#include <iostream>
#include <string>
#include "helpers.h"
#include "nlohmann.hpp"
#include "requests.h"

using json = nlohmann::json;

void login_admin(int sockfd, std::string host, std::string& session_cookie) {
    std::string username, password;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/admin/login", body_data, {});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Admin autentificat cu succes");
    } else {
        error_msg("Nu am putut autentifica adminul");
    }

    // session cookie
    size_t cookie_start =
        response.find("Set-Cookie: ") + 12;  // +12 to skip "Set-Cookie: "
    size_t cookie_end = response.find(";", cookie_start);
    session_cookie = response.substr(cookie_start, cookie_end - cookie_start);

    if (session_cookie.empty()) {
        error_msg("Nu am putut obtine cookie-ul");
    }
}

void DEBUG_login_admin(int sockfd,
                       std::string host,
                       std::string& session_cookie) {
    std::string username, password;
    username = "stefan.gatej";
    password = "930403745117";

    json body_data = {{"username", username}, {"password", password}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/admin/login", body_data, {});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Admin autentificat cu succes");
    } else {
        error_msg("Nu am putut autentifica adminul");
    }

    // session cookie
    size_t cookie_start =
        response.find("Set-Cookie: ") + 12;  // +12 to skip "Set-Cookie: "
    size_t cookie_end = response.find(";", cookie_start);
    session_cookie = response.substr(cookie_start, cookie_end - cookie_start);

    if (session_cookie.empty()) {
        error_msg("Nu am putut obtine cookie-ul");
    }
}

void add_user(int sockfd, std::string host, std::string session_cookie) {
    std::string username, password;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request = compute_post_request(host, "/api/v1/tema/admin/users",
                                               body_data, {session_cookie});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    // 201 = created
    // 209 = conflict (userul deja exista)

    if (status_code(response, 201)) {
        success_msg("User adaugat cu succes");
    } else if (status_code(response, 209)) {
        success_msg("Userul exista deja");
    } else {
        error_msg("Nu am putut adauga userul");
    }
}

void get_users(int sockfd, std::string host, std::string session_cookie) {
    std::string request = compute_get_request(host, "/api/v1/tema/admin/users",
                                              {}, {session_cookie});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200)) {
        success_msg("Useri obtinuti cu succes");
    } else {
        error_msg("Nu am putut obtine userii");
    }
}

void delete_user(int sockfd, std::string host, std::string session_cookie) {
    std::string username;
    std::cout << "username=";
    std::cin >> username;
    std::string path = "/api/v1/tema/admin/users/" + username;
    std::string request = compute_delete_request(host, path, {session_cookie});

    std::cout << request << std::endl;

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;

    if (status_code(response, 200) || status_code(response, 204)) {
        success_msg("User sters cu succes");
    } else if (status_code(response, 404)) {
        success_msg("Userul nu exista (not found)");
    } else if (status_code(response, 401) || status_code(response, 403)) {
        success_msg("Userul nu are permisiunea de a sterge");
    } else {
        error_msg("Nu am putut sterge userul");
    }
}

void logout_admin(int sockfd, std::string host, std::string& session_cookie) {
    std::string request = compute_get_request(host, "/api/v1/tema/admin/logout",
                                              {}, {session_cookie});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        session_cookie = "";  // clear the cookie
        success_msg("Adminul a fost delogat cu succes");
    } else {
        error_msg("Nu am putut deloga adminul");
    }
}

void login(int sockfd, std::string host, std::string& session_cookie) {
    std::string admin_username, username, password;
    std::cout << "admin_username=";
    std::cin >> admin_username;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"admin_username", admin_username},
                      {"username", username},
                      {"password", password}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/user/login", body_data, {});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Login cu succes");
    } else {
        error_msg("Nu am putut loga in contul " + username);
    }

    // session cookie
    size_t cookie_start = response.find("Set-Cookie: ") + 12;
    size_t cookie_end = response.find(";", cookie_start);
    session_cookie = response.substr(cookie_start, cookie_end - cookie_start);

    if (session_cookie.empty()) {
        error_msg("Nu am putut obtine cookie-ul pentru user");
    }
}

void get_access(int sockfd,
                std::string host,
                std::string session_cookie,
                std::string jwt_token) {
    std::string request = compute_get_request(
        host, "/api/v1/tema/library/access", {}, {session_cookie});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Accesul a fost obtinut cu succes");
    } else {
        error_msg("Nu am putut obtine accesul");
    }

    // parse response as json and retrieve "token" attribute

    json response_json = json::parse(response);
    jwt_token = response_json["token"];
}

int main() {
    std::string IP = "63.32.125.183";
    int PORT = 8081;

    std::string host = IP + ":" + std::to_string(PORT);
    std::string admin_session_cookie = "";
    std::string user_session_cookie = "";
    std::string jwt_token = "";

    int sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
    // login_admin(sockfd, host, session_cookie);

    // TODO: REMOVE THIS
    // DEBUG_login_admin(sockfd, host, session_cookie);

    // add_user(sockfd, host, session_cookie);
    // get_users(sockfd, host, admin_session_cookie);
    // delete_user(sockfd, host, session_cookie);
    // logout_admin(sockfd, host, admin_session_cookie);
    login(sockfd, host, user_session_cookie);
    get_access(sockfd, host, user_session_cookie, jwt_token);

    close_conn(sockfd);
    return 0;
}