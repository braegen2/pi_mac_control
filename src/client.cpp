#include <pi_mac_control/client.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

namespace pi_mac_control {

Client::Client(const std::string& server_ip, int port)
    : server_ip_(server_ip)
    , client_socket_(-1)
    , port_(port) {
}

Client::~Client() {
    cleanup();
}

bool Client::connect() {
    return initializeClient();
}

void Client::disconnect() {
    cleanup();
}

bool Client::sendCommand(const std::string& command) {
    if (client_socket_ < 0) {
        return false;
    }

    ssize_t sent = send(client_socket_, command.c_str(), command.length(), 0);
    return sent == static_cast<ssize_t>(command.length());
}

std::string Client::getResponse() {
    if (client_socket_ < 0) {
        return "";
    }

    char buffer[1024];
    int bytes_read = recv(client_socket_, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read <= 0) {
        return "";
    }
    
    buffer[bytes_read] = '\0';
    return std::string(buffer);
}

cv::Mat Client::getNextFrame() {
    if (client_socket_ < 0) {
        return cv::Mat();
    }

    // Receive frame size
    uint32_t frame_size;
    if (recv(client_socket_, &frame_size, sizeof(frame_size), 0) <= 0) {
        return cv::Mat();
    }

    // Receive frame data
    std::vector<uchar> buffer(frame_size);
    int total_received = 0;
    while (total_received < frame_size) {
        int received = recv(client_socket_, buffer.data() + total_received, frame_size - total_received, 0);
        if (received <= 0) {
            return cv::Mat();
        }
        total_received += received;
    }

    // Decode frame
    return cv::imdecode(buffer, cv::IMREAD_COLOR);
}

bool Client::initializeClient() {
    // Create socket
    client_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_ < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    // Connect to server
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, server_ip_.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return false;
    }

    std::cout << "Connecting to " << server_ip_ << " on port " << port_ << std::endl;
    if (::connect(client_socket_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return false;
    }

    return true;
}

void Client::cleanup() {
    if (client_socket_ >= 0) {
        close(client_socket_);
        client_socket_ = -1;
    }
}

} // namespace pi_mac_control 