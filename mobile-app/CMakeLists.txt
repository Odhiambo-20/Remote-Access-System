cmake_minimum_required(VERSION 3.16)
project(RemoteAccessMobile VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Android-specific Qt path configuration
if(ANDROID)
    set(CMAKE_PREFIX_PATH "/home/victor/Qt/6.9.3/android_arm64_v8a" CACHE PATH "Qt Android installation path" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

qt_add_executable(RemoteAccessMobile
    src/main.cpp
    src/connection_manager.cpp
    src/connection_manager.h
    src/pcmanager.cpp
    src/pcmanager.h
    src/filemanager.cpp
    src/filemanager.h
    src/settings_manager.cpp
    include/settings_manager.h
    src/remote_control_client.cpp
    include/remote_control_client.h
    src/pc_list_model.cpp
    src/pc_list_model.h
)

qt_add_qml_module(RemoteAccessMobile
    URI RemoteAccess
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/FileExplorerView.qml
        qml/PCListView.qml
        qml/AddPCView.qml
        qml/SettingsView.qml
        qml/components/FileItem.qml
        qml/components/PCCard.qml
        qml/components/ProgressBar.qml
)

target_link_libraries(RemoteAccessMobile PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
)

set_target_properties(RemoteAccessMobile PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Android-specific settings
if(ANDROID)
    set_target_properties(RemoteAccessMobile PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
        QT_ANDROID_ABIS "arm64-v8a"
    )
    
    # Set the correct library name for Android
    set_target_properties(RemoteAccessMobile PROPERTIES
        OUTPUT_NAME "RemoteAccessMobile_${ANDROID_ABI}"
    )
endif()

install(TARGETS RemoteAccessMobile
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
