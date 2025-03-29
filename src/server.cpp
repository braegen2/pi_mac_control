#include <pi_mac_control/server.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

namespace pi_mac_control {

Server::Server(int port) 
    : video_quality_(80)
    , frame_rate_(30)
    , running_(false)
    , server_socket_(-1)
    , client_socket_(-1)
    , port_(port) {
}

Server::~Server() {
    cleanup();
}

void Server::setVideoSource(int device_id) {
    camera_.open(device_id);
    if (!camera_.isOpened()) {
        throw std::runtime_error("Failed to open camera");
    }
}

void Server::setVideoQuality(int quality) {
    video_quality_ = std::clamp(quality, 0, 100);
}

void Server::setFrameRate(int fps) {
    frame_rate_ = fps;
}

void Server::start() {
    if (!initializeServer()) {
        throw std::runtime_error("Failed to initialize server");
    }

    running_ = true;
    video_thread_ = std::thread(&Server::videoStreamLoop, this);
}

std::string Server::getNextCommand() {
    char buffer[1024];
    int bytes_read = recv(client_socket_, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read <= 0) {
        return "";
    }
    
    buffer[bytes_read] = '\0';
    return std::string(buffer);
}

bool Server::sendResponse(const std::string& response) {
    if (client_socket_ < 0) {
        return false;
    }

    ssize_t sent = send(client_socket_, response.c_str(), response.length(), 0);
    return sent == static_cast<ssize_t>(response.length());
}

void Server::videoStreamLoop() {
    cv::Mat frame;
    while (running_) {
        camera_ >> frame;
        if (frame.empty()) {
            std::cerr << "Failed to grab frame" << std::endl;
            break;
        }

        // Convert frame to JPEG
        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer, {cv::IMWRITE_JPEG_QUALITY, video_quality_});

        // Send frame size first
        uint32_t frame_size = buffer.size();
        send(client_socket_, &frame_size, sizeof(frame_size), 0);

        // Send frame data
        send(client_socket_, buffer.data(), frame_size, 0);

        // Control frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / frame_rate_));
    }
}

bool Server::initializeServer() {
    // Create socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return false;
    }

    // Bind socket
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed on port " << port_ << std::endl;
        return false;
    }

    // Listen for connections
    if (listen(server_socket_, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }

    std::cout << "Server listening on port " << port_ << std::endl;

    // Accept connection
    int addrlen = sizeof(address);
    client_socket_ = accept(server_socket_, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_socket_ < 0) {
        std::cerr << "Accept failed" << std::endl;
        return false;
    }

    return true;
}

void Server::cleanup() {
    running_ = false;
    if (video_thread_.joinable()) {
        video_thread_.join();
    }
    
    if (client_socket_ >= 0) {
        close(client_socket_);
    }
    if (server_socket_ >= 0) {
        close(server_socket_);
    }
    
    camera_.release();
}

} // namespace pi_mac_control 