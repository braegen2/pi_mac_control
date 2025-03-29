cmake_minimum_required(VERSION 3.10)
project(pi_mac_control VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

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

# Link OpenCV and pthread
target_link_libraries(pi_mac_control
    PUBLIC
        ${OpenCV_LIBS}
        Threads::Threads
        pthread  # Explicitly link pthread on Linux
)

# Add examples
add_executable(pi_example examples/pi_example.cpp)
target_link_libraries(pi_example PRIVATE pi_mac_control)

add_executable(mac_example examples/mac_example.cpp)
target_link_libraries(mac_example PRIVATE pi_mac_control)

# Installation rules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CMakeConfigPackageHelpers)

# Install library and headers
install(TARGETS pi_mac_control
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/pi_mac_control
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install examples (optional)
install(TARGETS pi_example mac_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install CMake configuration files
install(DIRECTORY cmake/
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pi_mac_control
)

# Create and install the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/pi_mac_controlConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pi_mac_controlConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pi_mac_controlConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pi_mac_control
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/pi_mac_controlConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/pi_mac_controlConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pi_mac_control
)

cmake_minimum_required(VERSION 3.10)
project(test_pi_mac)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(pi_mac_control REQUIRED)

# Create test executables
add_executable(test_client test_client.cpp)
add_executable(test_server test_server.cpp)

# Link against the library
target_link_libraries(test_client
    PRIVATE
        pi_mac_control
        ${OpenCV_LIBS}
        Threads::Threads
)

target_link_libraries(test_server
    PRIVATE
        pi_mac_control
        ${OpenCV_LIBS}
        Threads::Threads
) 