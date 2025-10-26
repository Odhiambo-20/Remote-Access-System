cmake_minimum_required(VERSION 3.10)
project(RelayServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(relay_server
    src/relay_server_standalone.cpp
)

target_link_libraries(relay_server pthread)

install(TARGETS relay_server DESTINATION /usr/local/bin)
