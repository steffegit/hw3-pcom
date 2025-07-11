#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "nlohmann.hpp"

using json = nlohmann::json;

std::string compute_get_request(const std::string& host,
                                const std::string& path,
                                const json& body_data,
                                const std::vector<std::string>& cookies);

std::string compute_get_request(const std::string& host,
                                const std::string& path,
                                const json& body_data,
                                const std::vector<std::string>& cookies,
                                const std::string& jwt_token);

std::string compute_post_request(const std::string& host,
                                 const std::string& path,
                                 const json& body_data,
                                 const std::vector<std::string>& cookies);

std::string compute_post_request(const std::string& host,
                                 const std::string& path,
                                 const json& body_data,
                                 const std::vector<std::string>& cookies,
                                 const std::string& jwt_token);

std::string compute_delete_request(const std::string& host,
                                   const std::string& path,
                                   const std::vector<std::string>& cookies);

std::string compute_delete_request(const std::string& host,
                                   const std::string& path,
                                   const std::vector<std::string>& cookies,
                                   const std::string& jwt_token);

std::string compute_put_request(const std::string& host,
                                const std::string& path,
                                const json& body_data,
                                const std::vector<std::string>& cookies,
                                const std::string& jwt_token);
