#pragma once

#include <sys/socket.h>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include "helpers.h"
#include "nlohmann.hpp"
#include "requests.h"

using json = nlohmann::json;

// Admin operations
void login_admin(int& sockfd, std::string host, std::string& session_cookie);
void add_user(int& sockfd, std::string host, std::string session_cookie);
void get_users(int& sockfd, std::string host, std::string session_cookie);
void delete_user(int& sockfd, std::string host, std::string session_cookie);
void logout_admin(int& sockfd, std::string host, std::string& session_cookie);

// User operations
void login(int& sockfd, std::string host, std::string& session_cookie);
void get_access(int& sockfd,
                std::string host,
                std::string session_cookie,
                std::string& jwt_token);
void logout(int& sockfd,
            std::string host,
            std::string& session_cookie,
            std::string& jwt_token);

// Movie operations
void get_movies(int& sockfd,
                std::string host,
                std::string session_cookie,
                std::string jwt_token);
void get_movie(int& sockfd,
               std::string host,
               std::string session_cookie,
               std::string jwt_token);
void add_movie(int& sockfd,
               std::string host,
               std::string session_cookie,
               std::string jwt_token);
void delete_movie(int& sockfd,
                  std::string host,
                  std::string session_cookie,
                  std::string jwt_token);
void update_movie(int& sockfd,
                  std::string host,
                  std::string session_cookie,
                  std::string jwt_token);

// Collection operations
void get_collections(int& sockfd,
                     std::string host,
                     std::string session_cookie,
                     std::string jwt_token);
void get_collection(int& sockfd,
                    std::string host,
                    std::string session_cookie,
                    std::string jwt_token);
void add_collection(int& sockfd,
                    std::string host,
                    std::string session_cookie,
                    std::string jwt_token);
void delete_collection(int& sockfd,
                       std::string host,
                       std::string session_cookie,
                       std::string jwt_token);
void add_movie_to_collection(int& sockfd,
                             std::string host,
                             std::string session_cookie,
                             std::string jwt_token);
void delete_movie_from_collection(int& sockfd,
                                  std::string host,
                                  std::string session_cookie,
                                  std::string jwt_token);

void exit_client(int& sockfd);
