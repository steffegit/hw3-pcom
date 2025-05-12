#include <sys/socket.h>
// #include <charconv> -- maybe use this instead of stoi
#include <iostream>
#include <limits>
#include <string>
#include "helpers.h"
#include "nlohmann.hpp"
#include "requests.h"

using json = nlohmann::json;

std::unordered_map<int, std::string> movie_id_to_title;
std::unordered_map<int, std::pair<std::string, std::string>>
    collection_id_to_title_and_owner;

int running = 1;

void login_admin(int& sockfd, std::string host, std::string& session_cookie) {
    std::string username, password;
    std::cout << "username=";
    std::cin >> username;
    std::cout << "password=";
    std::cin >> password;

    json body_data = {{"username", username}, {"password", password}};

    std::string request =
        compute_post_request(host, "/api/v1/tema/admin/login", body_data, {});

    send_request(sockfd, host, request);
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

    send_request(sockfd, host, request);
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

    send_request(sockfd, host, request);
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

    send_request(sockfd, host, request);
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

            for (const auto& user : response_json["users"]) {
                std::cout << "#" << user["id"].get<int>() << " "
                          << user["username"].get<std::string>() << ":"
                          << user["password"].get<std::string>() << std::endl;
            }
        } catch (const std::exception& e) {
            error_msg("Nu am putut parsa raspunsul JSON");

            // std::cout << response << std::endl;  // TODO: REMOVE THIS !!!
            // DEBUG
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

    // std::cout << request << std::endl;

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    // std::cout << response << std::endl;

    if (status_code(response, 200)) {
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

    // std::cout << request << std::endl;

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    // std::cout << response << std::endl;

    if (status_code(response, 200)) {
        session_cookie = "";  // clear the cookie
        success_msg("Adminul a fost delogat cu succes");

        // TODO: WATCH OUT HERE (removed for now)

        // reset connection
        // IMPORTANT: THIS NEEDS TO BE DONE BECAUSE LOGOUT send a
        // "Connection: keep-alive" header

        // close_conn(sockfd);
        // std::string IP = host.substr(0, host.find(":"));
        // int PORT = std::stoi(host.substr(host.find(":") + 1));
        // sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
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

    send_request(sockfd, host, request);
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

    send_request(sockfd, host, request);
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
            // std::cout << e.what() << std::endl;

            // std::cout << response
            //           << std::endl;  // TODO: REMOVE THIS !!! DEBUG ONLY
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

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        // Extract the JSON body from the response
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        try {
            json response_json = json::parse(body);
            std::cout << "SUCCESS: Lista filmelor" << std::endl;

            // Clear the map before populating it
            movie_id_to_title.clear();

            // Store movies in a vector for sorting
            std::vector<std::pair<int, std::string>> movies_to_sort;
            for (const auto& movie : response_json["movies"]) {
                movies_to_sort.push_back({movie["id"].get<int>(),
                                          movie["title"].get<std::string>()});
            }

            // Sort by server ID (ascending)
            std::sort(movies_to_sort.begin(), movies_to_sort.end());

            // Display and store in map in sorted order
            for (const auto& movie : movies_to_sort) {
                std::cout << "#" << movie.first << " " << movie.second
                          << std::endl;
                movie_id_to_title[movie.first] = movie.second;
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

    send_request(sockfd, host, request);
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

    send_request(sockfd, host, request);
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

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
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

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Filmul a fost actualizat cu succes");
    } else {
        error_msg("Nu am putut actualiza filmul cu id-ul " + movie_id);
    }
}

void get_collections(int& sockfd,
                     std::string host,
                     std::string session_cookie,
                     std::string jwt_token) {
    std::string request =
        compute_get_request(host, "/api/v1/tema/library/collections", {},
                            {session_cookie}, jwt_token);

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        // Extract the JSON body from the response
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        try {
            json response_json = json::parse(body);
            std::cout << "SUCCESS: Lista colectiilor" << std::endl;

            // Clear the map before populating it
            collection_id_to_title_and_owner.clear();

            // Store collections in a vector for sorting
            std::vector<std::pair<int, std::string>> collections_to_sort;
            for (const auto& collection : response_json["collections"]) {
                collections_to_sort.push_back(
                    {collection["id"].get<int>(),
                     collection["title"].get<std::string>()});
            }

            // Sort by server ID (ascending)
            std::sort(collections_to_sort.begin(), collections_to_sort.end());

            // Display and store in map in sorted order
            for (const auto& collection : collections_to_sort) {
                std::cout << "#" << collection.first << ": "
                          << collection.second << std::endl;
                collection_id_to_title_and_owner[collection.first] =
                    std::make_pair(collection.second, collection.first);
            }
        } catch (const std::exception& e) {
            error_msg("Nu am putut parsa raspunsul JSON");
        }
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

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        size_t body_start = response.find("\r\n\r\n") + 4;
        std::string body = response.substr(body_start);

        try {
            json response_json = json::parse(body);

            // Get title and owner from the response JSON directly
            std::string title = response_json["title"].get<std::string>();
            std::string owner = response_json["owner"].get<std::string>();

            std::cout << "title: " << title << std::endl;
            std::cout << "owner: " << owner << std::endl;

            if (response_json.contains("movies") &&
                response_json["movies"].is_array()) {
                for (const auto& movie : response_json["movies"]) {
                    if (movie.contains("id") && movie.contains("title")) {
                        std::cout << "#" << movie["id"].get<int>() << ": "
                                  << movie["title"].get<std::string>()
                                  << std::endl;
                    }
                }
            }

            success_msg("Colectia a fost obtinuta cu succes");

        } catch (const std::exception& e) {
            error_msg("Nu am putut parsa raspunsul JSON");
        }

    } else {
        error_msg("Nu am putut obtine colectia cu id-ul " + collection_id);
    }
}

// add_collection -> primeste doar titlu, din care iau ID-ul colectiei
// in functie de numarul de filme, apelez add_movie_to_collection

void _add_collection_add_movie(int& sockfd,
                               std::string host,
                               std::string session_cookie,
                               std::string jwt_token,
                               std::string collection_id,
                               std::string title,
                               int movie_id) {
    json body_data = {{"id", movie_id}, {"title", title}};

    std::string request = compute_post_request(
        host, "/api/v1/tema/library/collections/" + collection_id + "/movies",
        body_data, {session_cookie}, jwt_token);

    send_request(sockfd, host, request);

    std::string response = recv_response(sockfd, host);
}

void add_collection(int& sockfd,
                    std::string host,
                    std::string session_cookie,
                    std::string jwt_token) {
    // Get and validate all input first
    std::string title;
    int num_movies;
    std::vector<int> movie_ids;

    // Get title
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "title=";
    std::getline(std::cin, title);
    if (title.empty()) {
        error_msg("Titlul nu poate fi gol");
        return;
    }

    // Get number of movies
    std::cout << "num_movies=";
    std::string num_movies_str;
    std::cin >> num_movies_str;
    try {
        num_movies = std::stoi(num_movies_str);
        if (num_movies <= 0) {
            error_msg("Numarul de filme trebuie sa fie pozitiv");
            return;
        }
    } catch (const std::exception& e) {
        error_msg("Numarul de filme trebuie sa fie un numar intreg");
        return;
    }

    // Get all movie IDs
    for (int i = 0; i < num_movies; i++) {
        std::cout << "movie_id[" << i << "]=";
        std::string movie_id_str;
        std::cin >> movie_id_str;
        try {
            int movie_id = std::stoi(movie_id_str);
            if (movie_id <= 0) {
                error_msg("Id-ul filmului trebuie sa fie pozitiv");
                return;
            }
            movie_ids.push_back(movie_id);
        } catch (const std::exception& e) {
            error_msg("Id-ul filmului trebuie sa fie un numar");
            return;
        }
    }

    // Now that all input is validated, proceed with requests
    // First create the collection with just the title
    json body_data = {{"title", title}};
    std::string request =
        compute_post_request(host, "/api/v1/tema/library/collections",
                             body_data, {session_cookie}, jwt_token);

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (!status_code(response, 201)) {  // 201 = CREATED
        error_msg("Nu am putut adauga colectia");
        return;
    }

    // Extract collection ID from response
    size_t body_start = response.find("\r\n\r\n") + 4;
    std::string body = response.substr(body_start);
    json response_json = json::parse(body);
    std::string collection_id = std::to_string(response_json["id"].get<int>());

    // Add all movies to collection
    for (int movie_id : movie_ids) {
        _add_collection_add_movie(sockfd, host, session_cookie, jwt_token,
                                  collection_id, title, movie_id);
    }

    // Get final collection details to display
    std::string request_get = compute_get_request(
        host, "/api/v1/tema/library/collections/" + collection_id, {},
        {session_cookie}, jwt_token);

    send_request(sockfd, host, request_get);
    std::string response_get = recv_response(sockfd, host);

    if (status_code(response_get, 200)) {
        size_t body_start = response_get.find("\r\n\r\n") + 4;
        std::string body = response_get.substr(body_start);
        json collection_data = json::parse(body);

        std::cout << "title: " << collection_data["title"].get<std::string>()
                  << std::endl;
        std::cout << "owner: " << collection_data["owner"].get<std::string>()
                  << std::endl;

        for (const auto& movie : collection_data["movies"]) {
            std::cout << "#" << movie["id"].get<int>() << ": "
                      << movie["title"].get<std::string>() << std::endl;
        }
    }

    // If you got here everything went well
    success_msg("Colectie adaugata cu succes");
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

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Colectia a fost stearsa cu succes");
    } else {
        error_msg("Nu am putut sterge colectia cu id-ul " + collection_id);
    }
}

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

    // Get collection details
    std::string request_get = compute_get_request(
        host, "/api/v1/tema/library/collections/" + collection_id, {},
        {session_cookie}, jwt_token);

    // std::cout << "GET Request:\n" << request_get << std::endl;

    send_request(sockfd, host, request_get);
    // std::cout << "GET Request sent successfully" << std::endl;

    std::string response_get = recv_response(sockfd, host);
    // std::cout << "GET Response received:\n" << response_get << std::endl;

    if (!status_code(response_get, 200)) {
        error_msg("Nu am putut obtine colectia cu id-ul " + collection_id +
                  " pentru a obtine titlul");
        return;
    }

    json collection_data =
        json::parse(response_get.substr(response_get.find("\r\n\r\n") + 4));
    std::string title = collection_data["title"].get<std::string>();

    int movie_id_int = std::stoi(movie_id);

    json body_data = {{"id", movie_id_int}, {"title", title}};

    std::string request = compute_post_request(
        host, "/api/v1/tema/library/collections/" + collection_id + "/movies",
        body_data, {session_cookie}, jwt_token);

    // std::cout << "POST Request:\n" << request << std::endl;

    send_request(sockfd, host, request);
    // std::cout << "POST Request sent successfully" << std::endl;

    std::string response = recv_response(sockfd, host);
    // std::cout << "POST Response received:\n" << response << std::endl;

    if (status_code(response, 200) || status_code(response, 201)) {
        success_msg("Filmul a fost adaugat la colectie");
    } else {
        error_msg("Nu am putut adauga filmul la colectie");
    }
}

void delete_movie_from_collection(int& sockfd,
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

    std::string request =
        compute_delete_request(host,
                               "/api/v1/tema/library/collections/" +
                                   collection_id + "/movies/" + movie_id,
                               {session_cookie}, jwt_token);

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Filmul a fost sters din colectie");
    } else {
        error_msg("Nu am putut sterge filmul din colectie");
    }
}

// comanda exit
void exit_client(int& sockfd) {
    close_conn(sockfd);
    running = 0;
}

void logout(int& sockfd,
            std::string host,
            std::string& session_cookie,
            std::string& jwt_token) {
    std::string request = compute_get_request(host, "/api/v1/tema/user/logout",
                                              {}, {session_cookie}, jwt_token);

    send_request(sockfd, host, request);
    std::string response = recv_response(sockfd, host);

    if (status_code(response, 200)) {
        success_msg("Utilizatorul a fost delogat cu succes");
    } else {
        error_msg("Nu am putut deloga utilizatorul");
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

    // login(sockfd, host, user_session_cookie);
    // get_access(sockfd, host, user_session_cookie, jwt_token);
    // add_movie(sockfd, host, user_session_cookie, jwt_token);
    // get_movies(sockfd, host, user_session_cookie, jwt_token);
    // get_movie(sockfd, host, user_session_cookie, jwt_token);
    // delete_movie(sockfd, host, user_session_cookie, jwt_token);
    // update_movie(sockfd, host, user_session_cookie, jwt_token);
    // get_collections(sockfd, host, user_session_cookie, jwt_token);
    // get_collection(sockfd, host, user_session_cookie, jwt_token);
    // TODO: needs to be tested
    // add_collection(sockfd, host, user_session_cookie, jwt_token);
    // delete_collection(sockfd, host, user_session_cookie, jwt_token);
    // add_movie_to_collection(sockfd, host, user_session_cookie, jwt_token);
    // delete_movie_from_collection(sockfd, host, user_session_cookie,
    // jwt_token);
    // logout(sockfd, host, user_session_cookie, jwt_token);
    // close_conn(sockfd);

    std::string command;

    while (running) {
        std::cin >> command;

        if (command == "login_admin") {
            login_admin(sockfd, host, admin_session_cookie);
        } else if (command == "add_user") {
            add_user(sockfd, host, admin_session_cookie);
        } else if (command == "get_users") {
            get_users(sockfd, host, admin_session_cookie);
        } else if (command == "delete_user") {
            delete_user(sockfd, host, admin_session_cookie);
        } else if (command == "logout_admin") {
            logout_admin(sockfd, host, admin_session_cookie);
        } else if (command == "login") {
            login(sockfd, host, user_session_cookie);
        } else if (command == "get_access") {
            get_access(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "get_movies") {
            get_movies(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "get_movie") {
            get_movie(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "add_movie") {
            add_movie(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "delete_movie") {
            delete_movie(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "update_movie") {
            update_movie(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "get_collections") {
            get_collections(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "get_collection") {
            get_collection(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "add_collection") {
            add_collection(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "delete_collection") {
            delete_collection(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "add_movie_to_collection") {
            add_movie_to_collection(sockfd, host, user_session_cookie,
                                    jwt_token);
        } else if (command == "delete_movie_from_collection") {
            delete_movie_from_collection(sockfd, host, user_session_cookie,
                                         jwt_token);
        } else if (command == "logout") {
            logout(sockfd, host, user_session_cookie, jwt_token);
        } else if (command == "exit") {
            exit_client(sockfd);
        } else {
            std::cout << "Comanda invalida\n";
        }
    }

    return 0;
}