#include <pi_mac_control/client.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

int main(int argc, char* argv[]) {
    // Get server IP and port from command line or use defaults
    std::string server_ip = "192.168.1.100";  // Replace with your Pi's IP
    int port = 8080;
    
    if (argc > 1) {
        server_ip = argv[1];
    }
    if (argc > 2) {
        port = std::atoi(argv[2]);
    }
    
    // Create and configure the client
    pi_mac_control::Client client(server_ip, port);
    
    // Connect to the Pi
    std::cout << "Connecting to Pi..." << std::endl;
    if (!client.connect()) {
        std::cerr << "Failed to connect to Pi" << std::endl;
        return 1;
    }
    
    std::cout << "Connected! Type commands (LED_ON, LED_OFF, or 'quit' to exit):" << std::endl;
    
    // Flag to control video thread
    std::atomic<bool> running{true};
    
    // Video display thread
    std::thread video_thread([&client, &running]() {
        while (running) {
            cv::Mat frame = client.getNextFrame();
            if (!frame.empty()) {
                cv::imshow("Pi Camera Feed", frame);
                if (cv::waitKey(1) == 27) { // ESC key
                    running = false;
                    break;
                }
            }
        }
    });
    
    // Command input loop
    std::string command;
    while (running) {
        std::getline(std::cin, command);
        
        if (command == "quit") {
            running = false;
            break;
        }
        
        // Send command to Pi
        if (!client.sendCommand(command)) {
            std::cerr << "Failed to send command" << std::endl;
            running = false;
            break;
        }
        
        // Get and display server response
        std::string response = client.getResponse();
        if (!response.empty()) {
            std::cout << "Server response: " << response << std::endl;
        }
    }
    
    // Cleanup
    video_thread.join();
    client.disconnect();
    cv::destroyAllWindows();
    
    return 0;
} 