cmake_minimum_required(VERSION 3.10)
project(PCClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)

# Check for ACE
pkg_check_modules(ACE REQUIRED ACE)

# Check for QR code and PNG libraries
pkg_check_modules(QRENCODE REQUIRED libqrencode)
pkg_check_modules(PNG REQUIRED libpng)

# Find Qt5 (since you're using Qt)
find_package(Qt5 COMPONENTS Core Network REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ACE_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${QRENCODE_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/relay_client.cpp
    src/connection_manager.cpp
    src/account_manager.cpp
    src/file_handler.cpp
    src/file_server.cpp
    src/file_manager.cpp
    src/remote_control_server.cpp
    src/pc_identifier.cpp
    src/http_server.cpp
    src/qr_generator.cpp
)

# Header files (for IDE support)
set(HEADERS
    include/relay_client.h
    include/connection_manager.h
    include/account_manager.h
    include/file_handler.h
    include/file_server.h
    include/file_manager.h
    include/remote_control_server.h
    include/pc_identifier.h
    include/http_server.h
    include/qr_generator.h
)

# Create executable
add_executable(pc_client ${SOURCES} ${HEADERS})

# Enable Qt MOC (Meta-Object Compiler) for main.cpp
set_property(SOURCE src/main.cpp PROPERTY SKIP_AUTOMOC OFF)
set(CMAKE_AUTOMOC ON)

# Link directories
link_directories(
    ${ACE_LIBRARY_DIRS}
    ${QRENCODE_LIBRARY_DIRS}
    ${PNG_LIBRARY_DIRS}
)

# Link libraries
target_link_libraries(pc_client
    Qt5::Core
    Qt5::Network
    ${ACE_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_LIBRARIES}
    ${QRENCODE_LIBRARIES}
    ${PNG_LIBRARIES}
    ssl
    crypto
    m
    pthread
)

# Compiler flags
target_compile_options(pc_client PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
)

# Add definitions
target_compile_definitions(pc_client PRIVATE
    QT_NO_DEBUG_OUTPUT
)

# Installation
install(TARGETS pc_client DESTINATION bin)

# Print configuration summary
message(STATUS "===========================================")
message(STATUS "PC Client Configuration Summary")
message(STATUS "===========================================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "ACE Include: ${ACE_INCLUDE_DIRS}")
message(STATUS "ACE Libraries: ${ACE_LIBRARIES}")
message(STATUS "QREncode Include: ${QRENCODE_INCLUDE_DIRS}")
message(STATUS "QREncode Libraries: ${QRENCODE_LIBRARIES}")
message(STATUS "PNG Include: ${PNG_INCLUDE_DIRS}")
message(STATUS "PNG Libraries: ${PNG_LIBRARIES}")
message(STATUS "OpenSSL Include: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL Libraries: ${OPENSSL_LIBRARIES}")
message(STATUS "Qt5 Core: ${Qt5Core_VERSION}")
message(STATUS "Qt5 Network: ${Qt5Network_VERSION}")
message(STATUS "===========================================")
