cmake_minimum_required(VERSION 3.10)
project(pi_mac_control)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Create library
add_library(pi_mac_control
    src/server.cpp
    src/client.cpp
)

# Set include directories
target_include_directories(pi_mac_control
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link OpenCV
target_link_libraries(pi_mac_control
    PUBLIC
        ${OpenCV_LIBS}
)

# Add examples
add_executable(pi_example examples/pi_example.cpp)
target_link_libraries(pi_example PRIVATE pi_mac_control)

add_executable(mac_example examples/mac_example.cpp)
target_link_libraries(mac_example PRIVATE pi_mac_control) 