#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

namespace pi_mac_control {

class Server {
public:
    Server(int port = 8080);  // Constructor with optional port
    ~Server();

    // Video configuration
    void setVideoSource(int device_id);
    void setVideoQuality(int quality);  // 0-100
    void setFrameRate(int fps);

    // Server control
    void start();
    std::string getNextCommand();
    bool sendResponse(const std::string& response);

private:
    // Video streaming
    cv::VideoCapture camera_;
    int video_quality_;
    int frame_rate_;
    std::thread video_thread_;
    std::atomic<bool> running_;

    // Network
    int server_socket_;
    int client_socket_;
    int port_;

    // Internal methods
    void videoStreamLoop();
    bool initializeServer();
    void cleanup();
};

} // namespace pi_mac_control 