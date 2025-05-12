#include "helpers.h"

void error(const std::string& msg) {
    perror(msg.c_str());
    exit(1);
}

void success_msg(std::string msg) {
    std::cout << "SUCCESS: " << msg << std::endl;
}

void error_msg(std::string msg) {
    std::cout << "ERROR: " << msg << std::endl;
}

bool status_code(std::string response, int expected_code) {
    std::string line = response.substr(0, response.find("\r\n"));  // first line
    return line.find(std::to_string(expected_code)) != std::string::npos;
}

int open_conn(std::string host,
              int port,
              int iptype,
              int socket_type,
              int flags) {
    int sockfd = socket(iptype, socket_type, flags);
    if (sockfd == -1)
        error("Error creating socket");

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(host.c_str());

    if (connect(sockfd, (struct sockaddr*)&server_address,
                sizeof(server_address)) == -1)
        error("Error connecting to server");

    return sockfd;
}

void close_conn(int sockfd) {
    close(sockfd);
}

void send_request(int& sockfd, std::string host, std::string message) {
    // Verifică dacă socket-ul este valid
    if (sockfd == -1) {
        // Socket-ul nu este valid, deschide o nouă conexiune
        std::string IP = host.substr(0, host.find(":"));
        int PORT = std::stoi(host.substr(host.find(":") + 1));
        sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
    } else {
        // Verifică dacă socket-ul existent este încă valid
        int error_code = 0;
        socklen_t error_code_size = sizeof(error_code);

        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error_code,
                       &error_code_size) < 0 ||
            error_code != 0) {
            // Socket-ul are o eroare, închide și redeschide
            close_conn(sockfd);
            std::string IP = host.substr(0, host.find(":"));
            int PORT = std::stoi(host.substr(host.find(":") + 1));
            sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
        } else {
            // Verifică dacă conexiunea este încă deschisă
            char test_byte;
            ssize_t test_result =
                recv(sockfd, &test_byte, 1, MSG_PEEK | MSG_DONTWAIT);

            if (test_result == 0 ||
                (test_result < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
                // Conexiunea a fost închisă sau are erori
                close_conn(sockfd);
                std::string IP = host.substr(0, host.find(":"));
                int PORT = std::stoi(host.substr(host.find(":") + 1));
                sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
            }
        }
    }

    // Trimite mesajul
    size_t sent = 0;
    const size_t total = message.length();

    while (sent < total) {
        ssize_t bytes = write(sockfd, message.data() + sent, total - sent);

        if (bytes < 0) {
            if (errno == EPIPE || errno == ECONNRESET) {
                // Conexiunea a fost închisă în timpul trimiterii
                close_conn(sockfd);
                std::string IP = host.substr(0, host.find(":"));
                int PORT = std::stoi(host.substr(host.find(":") + 1));
                sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);

                // Reîncearcă trimiterea de la început
                sent = 0;
                continue;
            }
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            // Nu ar trebui să se întâmple în mod normal pentru write()
            break;
        }

        sent += static_cast<size_t>(bytes);
    }
}

std::string recv_response(int& sockfd, std::string host) {
    std::string buffer;
    buffer.reserve(BUFLEN);
    char response[BUFLEN];
    bool connection_closed = false;

    // Verifică dacă trebuie să stabilim o conexiune
    if (sockfd == -1) {
        std::string IP = host.substr(0, host.find(":"));
        int PORT = std::stoi(host.substr(host.find(":") + 1));
        sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
    }

    // Verifică dacă socket-ul este încă valid
    int error_code = 0;
    socklen_t error_code_size = sizeof(error_code);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error_code,
                   &error_code_size) < 0 ||
        error_code != 0) {
        // Socket-ul are o eroare, închide și redeschide
        if (sockfd != -1) {
            close_conn(sockfd);
        }
        std::string IP = host.substr(0, host.find(":"));
        int PORT = std::stoi(host.substr(host.find(":") + 1));
        sockfd = open_conn(IP, PORT, AF_INET, SOCK_STREAM, 0);
    }

    // Citește până avem header-ele complete
    size_t header_end = std::string::npos;
    while (header_end == std::string::npos) {
        ssize_t bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            if (errno == ECONNRESET || errno == EPIPE || errno == ENOTCONN) {
                connection_closed = true;
                break;
            }
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            // Conexiunea a fost închisă de server
            connection_closed = true;
            break;
        }

        buffer.append(response, bytes);
        header_end = buffer.find(HEADER_TERMINATOR);
    }

    // Dacă conexiunea s-a închis înainte de a primi header-ele complete
    if (header_end == std::string::npos) {
        if (sockfd != -1) {
            close_conn(sockfd);
            sockfd = -1;
        }
        return buffer;
    }

    // Verifică dacă avem "Connection: close" în header
    if (buffer.find("Connection: close") != std::string::npos) {
        connection_closed = true;
    }

    // Parsează Content-Length dacă există
    size_t content_length = 0;
    bool has_content_length = false;
    size_t content_length_pos = buffer.find(CONTENT_LENGTH);

    if (content_length_pos != std::string::npos) {
        content_length_pos += CONTENT_LENGTH_SIZE;
        size_t end_of_length = buffer.find("\r\n", content_length_pos);
        if (end_of_length != std::string::npos) {
            try {
                content_length = std::stoul(buffer.substr(
                    content_length_pos, end_of_length - content_length_pos));
                has_content_length = true;
            } catch (const std::exception& e) {
                std::cerr << "Invalid Content-Length header" << std::endl;
            }
        }
    }

    // Calculează cât din body am citit deja
    size_t body_start = header_end + HEADER_TERMINATOR_SIZE;
    size_t body_already_read = buffer.length() - body_start;

    // Citește restul body-ului dacă este necesar
    if (has_content_length && body_already_read < content_length) {
        size_t remaining = content_length - body_already_read;

        while (remaining > 0 && !connection_closed) {
            ssize_t bytes = read(sockfd, response, std::min(BUFLEN, remaining));

            if (bytes < 0) {
                if (errno == ECONNRESET || errno == EPIPE) {
                    connection_closed = true;
                    break;
                }
                error("ERROR reading response body from socket");
            }

            if (bytes == 0) {
                // Conexiunea a fost închisă prematur
                connection_closed = true;
                break;
            }

            buffer.append(response, bytes);
            remaining -= bytes;
        }
    }

    // Verifică dacă răspunsul este complet
    bool response_complete =
        !has_content_length || (body_start + content_length <= buffer.length());

    // Verifică starea conexiunii după ce am primit răspunsul complet
    if (!connection_closed && response_complete) {
        // Testează dacă conexiunea este încă deschisă folosind un recv cu
        // MSG_PEEK
        char test_byte;
        ssize_t test_result =
            recv(sockfd, &test_byte, 1, MSG_PEEK | MSG_DONTWAIT);

        if (test_result == 0) {
            // Conexiunea a fost închisă de server după ce răspunsul a fost
            // trimis complet
            connection_closed = true;
        } else if (test_result < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            // Eroare la testarea conexiunii
            connection_closed = true;
        }
    }

    // Gestionează închiderea conexiunii
    if (connection_closed) {
        if (sockfd != -1) {
            shutdown(sockfd, SHUT_RDWR);
            close_conn(sockfd);
            sockfd = -1;
        }
    }

    return buffer;
}
