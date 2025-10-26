include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common/include
    ${ACE_ROOT}
    ${ACE_ROOT}/ace
    ${TAO_ROOT}
    ${TAO_ROOT}/tao
)

add_executable(account-server
    src/main.cpp
    src/database_manager.cpp
    src/account_server_impl.cpp
    src/server_manager.cpp
    src/types.cpp
)

target_link_libraries(account-server
    common
    ${ACE_LIB}
    ${TAO_LIB}
    /home/victor/ace_build/ACE_wrappers/TAO/tao/PortableServer/libTAO_PortableServer.so
    OpenSSL::SSL
    OpenSSL::Crypto
    SQLite::SQLite3
)
