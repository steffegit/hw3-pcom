#include <sys/socket.h>
// #include <charconv> -- maybe use this instead of stoi
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
        // IMPORTANT: THIS NEEDS TO BE DONE BECAUSE LOGOUT send a
        // "Connection: keep-alive" header
        close_conn(sockfd);
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

void get_movies(int& sockfd,
                std::string host,
                std::string session_cookie,
                std::string jwt_token) {
    std::string request = compute_get_request(
        host, "/api/v1/tema/library/movies", {}, {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        // Extract the JSON body from the response
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        try {
            json response_json = json::parse(body);
            std::cout << "SUCCESS: Lista filmelor" << std::endl;

            int count = 1;
            for (const auto& movie : response_json["movies"]) {
                std::cout << "#" << count << " "
                          << movie["title"].get<std::string>() << std::endl;
                count++;
            }
        } catch (const std::exception& e) {
            error_msg("Nu am putut parsa raspunsul JSON");
        }
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

    if (status_code(response, 200)) {
        success_msg("Filmul a fost obtinut cu succes");

        // Extract the JSON body from the response
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        try {
            json response_json = json::parse(body);
            std::cout << response_json.dump(2) << std::endl;
        } catch (const std::exception& e) {
            error_msg("Nu am putut parsa raspunsul JSON");
        }
    } else {
        error_msg("Nu am putut obtine filmul cu id-ul " + movie_id);
    }
}

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

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 201)) {  // 201 = CREATED
        success_msg("Filmul a fost adaugat cu succes");
    } else {
        error_msg("Nu am putut adauga filmul");
    }
}

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

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

    std::cout << request << std::endl;

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

// add_collection -> primeste doar titlu, din care iau ID-ul colectiei
// in functie de numarul de filme, apelez add_movie_to_collection

// TODO: come back here
void add_collection(int& sockfd,
                    std::string host,
                    std::string session_cookie,
                    std::string jwt_token) {
    std::string title;
    int num_movies;
    std::vector<int> movie_ids;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "title=";
    std::getline(std::cin, title);

    std::cout << "num_movies=";
    std::cin >> num_movies;

    for (int i = 0; i < num_movies; i++) {
        std::cout << "movie_id[" << i << "]=";
        std::string movie_id_str;
        std::cin >> movie_id_str;
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

    std::cout << request << std::endl;

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    std::cout << response << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY

    if (status_code(response, 201)) {  // 201 = CREATED
        success_msg("Colectie adaugata");

        // Extract and display the response details
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        try {
            json response_json = json::parse(body);
            std::cout << "title: " << response_json["title"].get<std::string>()
                      << std::endl;
            std::cout << "owner: " << response_json["owner"].get<std::string>()
                      << std::endl;

            for (const auto& movie : response_json["movies"]) {
                std::cout << "#" << movie["id"].get<int>() << ": "
                          << movie["title"].get<std::string>() << std::endl;
            }
        } catch (const std::exception& e) {
            error_msg("Nu am putut parsa raspunsul JSON");
        }
    } else {
        error_msg("Nu am putut adauga colectia");
    }
}

void delete_collection(int& sockfd,
                       std::string host,
                       std::string session_cookie,
                       std::string jwt_token) {
    std::string collection_id;
    std::cout << "id=";
    std::cin >> collection_id;

    if (!std::all_of(collection_id.begin(), collection_id.end(), ::isdigit)) {
        error_msg("Id-ul colectiei trebuie sa fie un numar");
        return;
    }

    std::string request = compute_delete_request(
        host, "/api/v1/tema/library/collections/" + collection_id,
        {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200) || status_code(response, 204)) {
        success_msg("Colectia a fost stearsa cu succes");
    } else {
        error_msg("Nu am putut sterge colectia cu id-ul " + collection_id);
    }
}

// TODO: THIS NOT WORKS
void add_movie_to_collection(int& sockfd,
                             std::string host,
                             std::string session_cookie,
                             std::string jwt_token) {
    std::string collection_id;
    std::cout << "id=";
    std::cin >> collection_id;

    if (!std::all_of(collection_id.begin(), collection_id.end(), ::isdigit)) {
        error_msg("Id-ul colectiei trebuie sa fie un numar");
        return;
    }

    std::string movie_id;
    std::cout << "movie_id=";
    std::cin >> movie_id;

    if (!std::all_of(movie_id.begin(), movie_id.end(), ::isdigit)) {
        error_msg("Id-ul filmului trebuie sa fie un numar");
        return;
    }

    // grab the title of the collection

    std::string request_get = compute_get_request(
        host, "/api/v1/tema/library/collections/" + collection_id, {},
        {session_cookie}, jwt_token);
    send_request(sockfd, request_get);

    // std::cout << request_get << std::endl;

    std::string response_get = recv_response(sockfd, host);

    // std::cout << response_get << std::endl;

    if (!status_code(response_get, 200)) {
        error_msg("Nu am putut obtine colectia cu id-ul " + collection_id +
                  " pentru a obtine titlul");
        return;
    }

    json collection_data =
        json::parse(response_get.substr(response_get.find("\r\n\r\n") + 4));
    std::string title = collection_data["title"].get<std::string>();

    json body_data = {{"id", std::stoi(movie_id)}, {"title", title}};

    std::string request = compute_post_request(
        host, "/api/v1/tema/library/collections/" + collection_id + "/movies",
        body_data, {session_cookie}, jwt_token);

    send_request(sockfd, request);
    std::string response = recv_response(sockfd, host);

    // std::cout << response << std::endl;

    if (status_code(response, 201)) {
        success_msg("Filmul a fost adaugat la colectie");
    } else {
        error_msg("Nu am putut adauga filmul la colectie");
    }
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
    // DEBUG_login_admin(sockfd, host, admin_session_cookie);
    // add_user(sockfd, host, admin_session_cookie);
    // get_users(sockfd, host, admin_session_cookie);
    // logout_admin(sockfd, host, admin_session_cookie);

    login(sockfd, host, user_session_cookie);
    get_access(sockfd, host, user_session_cookie, jwt_token);
    // add_movie(sockfd, host, user_session_cookie, jwt_token);
    // get_movies(sockfd, host, user_session_cookie, jwt_token);
    // get_movie(sockfd, host, user_session_cookie, jwt_token);
    // delete_movie(sockfd, host, user_session_cookie, jwt_token);
    // update_movie(sockfd, host, user_session_cookie, jwt_token);
    // get_collections(sockfd, host, user_session_cookie, jwt_token);
    get_collection(sockfd, host, user_session_cookie, jwt_token);
    // TODO: needs to be tested
    // add_collection(sockfd, host, user_session_cookie, jwt_token);
    // delete_collection(sockfd, host, user_session_cookie, jwt_token);
    // add_movie_to_collection(sockfd, host, user_session_cookie, jwt_token);
    close_conn(sockfd);
    return 0;
}