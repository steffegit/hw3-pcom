#pragma once

#include <iostream>
#include <string>
#include <vector>

std::string compute_get_request(const std::string& host,
                                const std::string& path,
                                const std::string& query_params,
                                const std::vector<std::string>& cookies);
std::string compute_post_request(const std::string& host,
                                 const std::string& path,
                                 const std::string& content_type,
                                 const std::vector<std::string>& body_data,
                                 const std::vector<std::string>& cookies);
