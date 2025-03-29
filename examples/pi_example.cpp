#include <pi_mac_control/server.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    // Get port from command line or use default
    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }
    
    // Create and configure the server
    pi_mac_control::Server server(port);
    
    // Configure video streaming
    server.setVideoSource(0);  // Use default camera
    server.setVideoQuality(80); // JPEG quality (0-100)
    server.setFrameRate(30);   // FPS
    
    // Start the server
    std::cout << "Starting server..." << std::endl;
    server.start();
    
    // Command handling loop
    std::cout << "Server running. Commands will be printed to console." << std::endl;
    while (true) {
        std::string command = server.getNextCommand();
        if (!command.empty()) {
            std::cout << "Received command: " << command << std::endl;
            
            // Handle commands and send responses
            if (command == "LED_ON") {
                std::cout << "Turning LED ON" << std::endl;
                server.sendResponse("LED turned on successfully");
            } else if (command == "LED_OFF") {
                std::cout << "Turning LED OFF" << std::endl;
                server.sendResponse("LED turned off successfully");
            } else {
                server.sendResponse("Unknown command: " + command);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
} 