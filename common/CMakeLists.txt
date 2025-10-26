cmake_minimum_required(VERSION 3.16)
project(RemoteAccessCommon VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

set(PROJECT_SOURCES
    src/utils.cpp
    src/protocol.cpp
    src/crypto.cpp
    src/hardware_id.cpp
)

set(PROJECT_HEADERS
    include/utils.h
    include/protocol.h
    include/crypto.h
    include/hardware_id.h
)

add_library(common STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(common PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto
)
