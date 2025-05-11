#include <sys/socket.h>
#include <iostream>
#include <limits>
#include <string>
#include "helpers.h"
#include "nlohmann.hpp"
#include "requests.h"

using json = nlohmann::json;

void login_admin(int& sockfd, std::string host, std::string& session_cookie) {
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

void DEBUG_login_admin(int& sockfd,
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

void add_user(int& sockfd, std::string host, std::string session_cookie) {
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

void get_users(int& sockfd, std::string host, std::string session_cookie) {
    std::string request = compute_get_request(host, "/api/v1/tema/admin/users",
                                              {}, {session_cookie});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        // Extract the JSON body from the response
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        // strip the end
        body = body.substr(0, body.size() - 1);

        try {
            json response_json = json::parse(body);
            std::cout << "SUCCESS: Lista utilizatorilor" << std::endl;

            int count = 1;
            for (const auto& user : response_json["users"]) {
                std::cout << "#" << count << " "
                          << user["username"].get<std::string>() << ":"
                          << user["password"].get<std::string>() << std::endl;
                count++;
            }
        } catch (const std::exception& e) {
            error_msg("Nu am putut parsa raspunsul JSON");

            std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG
        }
    } else {
        error_msg("Nu am putut obtine userii");
    }
}

void delete_user(int& sockfd, std::string host, std::string session_cookie) {
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

void logout_admin(int& sockfd, std::string host, std::string& session_cookie) {
    std::string request = compute_get_request(host, "/api/v1/tema/admin/logout",
                                              {}, {session_cookie});

    std::cout << request << std::endl;

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;

    if (status_code(response, 200)) {
        session_cookie = "";  // clear the cookie
        success_msg("Adminul a fost delogat cu succes");

        // reset connection
        close(sockfd);
        // FIXME: THIS NEEDS TO BE DONE BECAUSE LOGOUT send a
        // "Connection: keep-alive" header
        std::string IP = host.substr(0, host.find(":"));
        int PORT = std::stoi(host.substr(host.find(":") + 1));
        sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
    } else {
        error_msg("Nu am putut deloga adminul");
    }
}

void login(int& sockfd, std::string host, std::string& session_cookie) {
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

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200)) {
        success_msg("Login cu succes");
    } else {
        error_msg("Nu am putut loga in contul " + username);
        return;
    }

    // session cookie
    size_t cookie_start = response.find("Set-Cookie: ") + 12;
    size_t cookie_end = response.find(";", cookie_start);
    session_cookie = response.substr(cookie_start, cookie_end - cookie_start);

    if (session_cookie.empty()) {
        error_msg("Nu am putut obtine cookie-ul pentru user");
    }
}

void get_access(int& sockfd,
                std::string host,
                std::string session_cookie,
                std::string& jwt_token) {
    std::string request = compute_get_request(
        host, "/api/v1/tema/library/access", {}, {session_cookie});

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Accesul a fost obtinut cu succes");

        // Extract the token from the JSON response
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        try {
            json response_json = json::parse(body);
            jwt_token = response_json["token"];
        } catch (const std::exception& e) {
            error_msg("Nu am putut obtine accesul (eroare parsare json)");
            std::cout << e.what() << std::endl;

            std::cout << response
                      << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY
        }
    } else {
        error_msg("Nu am putut obtine accesul");
    }
}

// TODO: revino aici, trebuie sa faci formatul ca aici:
// https://pcom.pages.upb.ro/enunt-tema4/client.html#7-get_movies-10p
void get_movies(int& sockfd,
                std::string host,
                std::string session_cookie,
                std::string jwt_token) {
    std::string request = compute_get_request(
        host, "/api/v1/tema/library/movies", {}, {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200)) {
        success_msg("Filmele au fost obtinute cu succes");
    } else {
        error_msg("Nu am putut obtine filmele");
    }
}

void get_movie(int& sockfd,
               std::string host,
               std::string session_cookie,
               std::string jwt_token) {
    std::string movie_id;
    std::cout << "id=";
    std::cin >> movie_id;

    // check if movie_id is a number
    if (!std::all_of(movie_id.begin(), movie_id.end(), ::isdigit)) {
        error_msg("Id-ul filmului trebuie sa fie un numar");
        return;
    }

    std::string request =
        compute_get_request(host, "/api/v1/tema/library/movies/" + movie_id, {},
                            {session_cookie}, jwt_token);

    send_request(sockfd, request);

    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200)) {
        success_msg("Filmul a fost obtinut cu succes");
    } else {
        error_msg("Nu am putut obtine filmul cu id-ul " + movie_id);
    }
}

// TODO: server error here, come back to this
void add_movie(int& sockfd,
               std::string host,
               std::string session_cookie,
               std::string jwt_token) {
    // year should be int
    // rating should be float
    std::string title, description;
    int year;
    float rating;

    std::cout << "title=";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, title);

    std::cout << "year=";
    std::string year_str;
    std::getline(std::cin, year_str);
    try {
        year = std::stoi(year_str);
    } catch (const std::exception& e) {
        error_msg("Anul trebuie sa fie un numar intreg");
        return;
    }

    std::cout << "description=";
    std::getline(std::cin, description);

    std::cout << "rating=";
    std::string rating_str;
    std::getline(std::cin, rating_str);
    try {
        rating = std::stof(rating_str);
    } catch (const std::exception& e) {
        error_msg("Rating-ul trebuie sa fie un numar real");
        return;
    }

    json body_data = {{"title", title},
                      {"year", year},
                      {"description", description},
                      {"rating", rating}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/library/movies", body_data,
                             {session_cookie}, jwt_token);

    std::cout << request << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 201)) {
        success_msg("Filmul a fost adaugat cu succes");
    } else {
        error_msg("Nu am putut adauga filmul");
    }
}

// TODO: test this
void delete_movie(int& sockfd,
                  std::string host,
                  std::string session_cookie,
                  std::string jwt_token) {
    std::string movie_id;
    std::cout << "id=";
    std::cin >> movie_id;

    // check if movie_id is a number
    if (!std::all_of(movie_id.begin(), movie_id.end(), ::isdigit)) {
        error_msg("Id-ul filmului trebuie sa fie un numar");
        return;
    }

    std::string request =
        compute_delete_request(host, "/api/v1/tema/library/movies/" + movie_id,
                               {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200) || status_code(response, 204)) {
        success_msg("Filmul a fost sters cu succes");
    } else {
        error_msg("Nu am putut sterge filmul cu id-ul " + movie_id);
    }
}

void update_movie(int& sockfd,
                  std::string host,
                  std::string session_cookie,
                  std::string jwt_token) {
    std::string movie_id;
    std::cout << "id=";
    std::cin >> movie_id;

    // check if movie_id is a number
    if (!std::all_of(movie_id.begin(), movie_id.end(), ::isdigit)) {
        error_msg("Id-ul filmului trebuie sa fie un numar");
        return;
    }

    std::string title, description;
    int year;
    float rating;

    std::cout << "title=";
    std::getline(std::cin, title);

    std::cout << "year=";
    std::string year_str;
    std::getline(std::cin, year_str);
    try {
        year = std::stoi(year_str);
    } catch (const std::exception& e) {
        error_msg("Anul trebuie sa fie un numar intreg");
        return;
    }

    std::cout << "description=";
    std::getline(std::cin, description);

    std::cout << "rating=";
    std::string rating_str;
    std::getline(std::cin, rating_str);
    try {
        rating = std::stof(rating_str);
    } catch (const std::exception& e) {
        error_msg("Rating-ul trebuie sa fie un numar real");
        return;
    }

    json body_data = {{"title", title},
                      {"year", year},
                      {"description", description},
                      {"rating", rating}};

    std::string request =
        compute_put_request(host, "/api/v1/tema/library/movies/" + movie_id,
                            body_data, {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200) || status_code(response, 204)) {
        success_msg("Filmul a fost actualizat cu succes");
    } else {
        error_msg("Nu am putut actualiza filmul cu id-ul " + movie_id);
    }
}

// TODO: test + parsare response
// https://pcom.pages.upb.ro/enunt-tema4/client.html#12-get_collections-10p
void get_collections(int& sockfd,
                     std::string host,
                     std::string session_cookie,
                     std::string jwt_token) {
    std::string request =
        compute_get_request(host, "/api/v1/tema/library/collections", {},
                            {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200)) {
        success_msg("Colectiile au fost obtinute cu succes");
    } else {
        error_msg("Nu am putut obtine colectiile");
    }
}

void get_collection(int& sockfd,
                    std::string host,
                    std::string session_cookie,
                    std::string jwt_token) {
    std::string collection_id;
    std::cout << "id=";
    std::cin >> collection_id;

    // check if collection_id is a number
    if (!std::all_of(collection_id.begin(), collection_id.end(), ::isdigit)) {
        error_msg("Id-ul colectiei trebuie sa fie un numar");
        return;
    }

    std::string request = compute_get_request(
        host, "/api/v1/tema/library/collections/" + collection_id, {},
        {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 200)) {
        success_msg("Colectia a fost obtinuta cu succes");
    } else {
        error_msg("Nu am putut obtine colectia cu id-ul " + collection_id);
    }
}

// TODO: THIS IS WRONG
void add_collection(int& sockfd,
                    std::string host,
                    std::string session_cookie,
                    std::string jwt_token) {
    std::string title;
    int num_movies;
    std::vector<int> movie_ids;

    std::cout << "title=";
    std::getline(std::cin, title);

    std::cout << "num_movies=";
    std::cin >> num_movies;

    for (int i = 0; i < num_movies; i++) {
        std::cout << "movie_id[" << i << "]=";
        std::string movie_id_str;
        std::getline(std::cin, movie_id_str);
        try {
            int movie_id = std::stoi(movie_id_str);
            movie_ids.push_back(movie_id);
        } catch (const std::exception& e) {
            error_msg("Id-ul filmului trebuie sa fie un numar");
            return;
        }
    }

    json body_data = {
        {"title", title}, {"num_movies", num_movies}, {"movie_ids", movie_ids}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/library/collections",
                             body_data, {session_cookie}, jwt_token);

    send_request(sockfd, request);
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
    DEBUG_login_admin(sockfd, host, admin_session_cookie);
    // add_user(sockfd, host, admin_session_cookie);
    get_users(sockfd, host, admin_session_cookie);
    logout_admin(sockfd, host, admin_session_cookie);

    login(sockfd, host, user_session_cookie);
    get_access(sockfd, host, user_session_cookie, jwt_token);
    add_movie(sockfd, host, user_session_cookie, jwt_token);
    get_movies(sockfd, host, user_session_cookie, jwt_token);
    get_movie(sockfd, host, user_session_cookie, jwt_token);
    close_conn(sockfd);
    return 0;
}