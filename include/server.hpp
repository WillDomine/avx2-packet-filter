#pragma once
#include <liburing.h>
#include <netinet/in.h>
#include <vector>

#define QUEUE_DEPTH 512
#define BUFFER_SIZE 1024

// Enum to track what kind of request completed
enum class RequestType {
    ACCEPT,
    READ,
    WRITE
};

// Context for every IO operation
struct Request {
    RequestType type;
    int client_fd;
    std::vector<char> buffer;
    struct iovec iov;

    Request(RequestType t, int fd) : type(t), client_fd(fd) {
        buffer.resize(BUFFER_SIZE);
        iov.iov_base = buffer.data();
        iov.iov_len = BUFFER_SIZE;
    }
};

class Server {
public:
    Server(int port);
    ~Server();

    // Initialize Ring and Socket
    bool setup();
    
    // Start the Event Loop
    void run();

private:
    int port;
    int server_fd;
    struct io_uring ring;
    
    // Helpers
    void add_accept_request(struct sockaddr_in *client_addr, socklen_t *client_len);
    void add_read_request(int client_fd);
    
    // Handlers
    void handle_accept(struct io_uring_cqe *cqe);
    void handle_read(struct io_uring_cqe *cqe);
};