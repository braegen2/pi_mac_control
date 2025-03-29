#pragma once

#include <string>
#include <opencv2/opencv.hpp>

namespace pi_mac_control {

class Client {
public:
    explicit Client(const std::string& server_ip, int port = 8080);  // Constructor with optional port
    ~Client();

    // Connection control
    bool connect();
    void disconnect();

    // Communication
    bool sendCommand(const std::string& command);
    std::string getResponse();  // Get response from last command
    cv::Mat getNextFrame();

private:
    // Network
    std::string server_ip_;
    int client_socket_;
    int port_;

    // Internal methods
    bool initializeClient();
    void cleanup();
};

} // namespace pi_mac_control 