cmake_minimum_required(VERSION 3.10)
project(PCClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ACE
find_library(ACE_LIBRARY NAMES ACE PATHS /usr/lib /usr/local/lib)
find_path(ACE_INCLUDE_DIR ace/ACE.h PATHS /usr/include /usr/local/include)

if(NOT ACE_LIBRARY OR NOT ACE_INCLUDE_DIR)
    message(FATAL_ERROR "ACE library not found")
endif()

include_directories(${ACE_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

# PC Client executable
add_executable(pc_client
    pc_client_main.cpp
    pc_client.cpp
    file_handler.cpp
    src/http_server.cpp
)

target_link_libraries(pc_client ${ACE_LIBRARY} pthread)

install(TARGETS pc_client DESTINATION bin)
