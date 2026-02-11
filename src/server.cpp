#include "../include/server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

Server::Server(int p) : port(p), server_fd(-1) {}

Server::~Server() {
    if (server_fd != -1) close(server_fd);
    io_uring_queue_exit(&ring);
};

bool Server::setup() {
    //Set up the socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    {
        perror("Socket Connection Failed");
        return false;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr) < 0))
    {
        perror("Bind failed");
        return false;
    }
    
}