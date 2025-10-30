cmake_minimum_required(VERSION 3.16)
project(pc_client)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt5 REQUIRED COMPONENTS Core Network)
find_package(OpenSSL REQUIRED)

# Set Qt auto-generation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ACE library
find_library(ACE_LIBRARY ACE PATHS /usr/local/lib /usr/lib)
find_path(ACE_INCLUDE_DIR ace/ACE.h PATHS /usr/local/include /usr/include)

# QREncode library
find_library(QRENCODE_LIBRARY qrencode)

# PNG library
find_package(PNG REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${ACE_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${PNG_INCLUDE_DIRS}
)

# Compiler flags
add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter -fPIC)

# Header files for PC client
set(PC_CLIENT_HEADERS
    include/pc_identifier.h
    include/remote_control_server.h
    include/file_server.h
    include/relay_client.h
    include/connection_manager.h
    include/account_manager.h
    include/file_handler.h
    include/file_manager.h
    include/http_server.h
    include/qr_generator.h
)

# Source files for PC client
set(PC_CLIENT_SOURCES
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

# Common source files (utils and crypto)
set(COMMON_SOURCES
    ../common/src/utils.cpp
    ../common/src/crypto.cpp
)

# Create executable - INCLUDE HEADERS HERE
add_executable(pc_client 
    ${PC_CLIENT_HEADERS}
    ${PC_CLIENT_SOURCES}
    ${COMMON_SOURCES}
)

# Link libraries
target_link_libraries(pc_client
    Qt5::Core
    Qt5::Network
    ${ACE_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${QRENCODE_LIBRARY}
    ${PNG_LIBRARIES}
    ssl
    crypto
    m
    pthread
)

# Print configuration summary
message(STATUS "===========================================")
message(STATUS "PC Client Configuration Summary")
message(STATUS "===========================================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "ACE Include: ${ACE_INCLUDE_DIR}")
message(STATUS "ACE Libraries: ${ACE_LIBRARY}")
message(STATUS "QREncode Include: ${QRENCODE_INCLUDE_DIR}")
message(STATUS "QREncode Libraries: ${QRENCODE_LIBRARY}")
message(STATUS "PNG Include: ${PNG_INCLUDE_DIRS}")
message(STATUS "PNG Libraries: ${PNG_LIBRARIES}")
message(STATUS "OpenSSL Include: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL Libraries: ${OPENSSL_LIBRARIES}")
message(STATUS "Qt5 Core: ${Qt5Core_VERSION}")
message(STATUS "Qt5 Network: ${Qt5Network_VERSION}")
message(STATUS "===========================================")