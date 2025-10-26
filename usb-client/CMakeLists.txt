cmake_minimum_required(VERSION 3.10)
project(USBClient)

set(CMAKE_CXX_STANDARD 17)

# Find ACE/TAO
find_library(ACE_LIBRARY NAMES ACE)
find_library(TAO_LIBRARY NAMES TAO)
find_library(TAO_PORTABLESERVER NAMES TAO_PortableServer)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
)

# Find required packages
find_package(OpenSSL REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/config_manager.cpp
    src/usb_manager.cpp
    ../common/src/protocol.cpp
    ../common/src/crypto.cpp
    ../common/src/utils.cpp
)

# Create executable
add_executable(usb-client ${SOURCES})

# Link libraries
target_link_libraries(usb-client
    ${ACE_LIBRARY}
    ${TAO_LIBRARY}
    ${TAO_PORTABLESERVER}
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)
