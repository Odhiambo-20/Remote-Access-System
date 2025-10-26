cmake_minimum_required(VERSION 3.10)
project(RemoteAccessSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Threads
find_package(Threads REQUIRED)

# X11 libraries
find_package(X11 REQUIRED)

# OpenSSL for password hashing
find_package(OpenSSL REQUIRED)

# Create output directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ============================================
# PROTOCOL LIBRARY (COMMON)
# ============================================
add_library(protocol STATIC
    common/src/protocol.cpp
)
target_include_directories(protocol PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common/include
)
target_link_libraries(protocol PUBLIC
    Threads::Threads
)

# ============================================
# ACCOUNT SERVER
# ============================================
add_executable(account-server
    account-server/src/main.cpp
    account-server/src/account_manager.cpp
)
target_include_directories(account-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/account-server/include
)
target_link_libraries(account-server
    protocol
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# ============================================
# RELAY SERVER
# ============================================
add_executable(relay-server
    relay-server/src/main.cpp
    relay-server/src/relay_manager.cpp
)
target_include_directories(relay-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/relay-server/include
)
target_link_libraries(relay-server
    protocol
    Threads::Threads
)

# ============================================
# PC CLIENT
# ============================================
add_executable(pc-client
    pc-client/src/main.cpp
    pc-client/src/pc_identifier.cpp
    pc-client/src/remote_control_server.cpp
    pc-client/src/file_server.cpp
)
target_include_directories(pc-client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pc-client/include
)
target_link_libraries(pc-client
    protocol
    Threads::Threads
    X11::X11
    Xtst
)

# ============================================
# OUTPUT DIRECTORIES
# ============================================
set_target_properties(account-server relay-server pc-client
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ============================================
# INSTALL TARGET (OPTIONAL)
# ============================================
install(TARGETS account-server relay-server pc-client
    RUNTIME DESTINATION bin
)

