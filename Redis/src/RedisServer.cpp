#include "../include/RedisServer.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

static RedisServer* server_instance = nullptr;

RedisServer::RedisServer(int port) : port_(port), server_socket_(-1), running_(false) {
    server_instance = this;
}

void RedisServer::shutdown() {
    running_ = false;

    if (server_socket_ != -1) {
        close(server_socket_);
        server_socket_ = -1;
    }
}

void RedisServer::run() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    int val = 1;
    setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = port_;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket_, (const struct sockaddr*) &addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return;
    }

    if (listen(server_socket_, SOMAXCONN)) {
        std::cerr << "Failed to listen socket" << std::endl;
        return;
    }

    // change 
    while (true) {
        struct sockaddr_in client_addr = {};
        socklen_t client_addr_len = sizeof(addr);

        int connected_socket = accept(server_socket_, (struct sockaddr*) &client_addr, &client_addr_len);

        if (connected_socket < 0) {
            std::cerr << "Error accepting socket" << std::endl;
            return;
        }

        // reading the socket

        char buffer[1024] = {0};
        recv(connected_socket, buffer, sizeof(buffer), 0);
        std::cout << "Message from client: " << buffer;
    }
}

