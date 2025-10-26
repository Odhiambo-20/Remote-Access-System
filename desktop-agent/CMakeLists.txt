cmake_minimum_required(VERSION 3.10)
project(DesktopAgent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ACE REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(desktop_agent desktop_agent.cpp)

target_link_libraries(desktop_agent
    ACE::ACE
    OpenSSL::SSL
    OpenSSL::Crypto
)
