QT += quick quickcontrols2 network websockets
CONFIG += c++17

VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TARGET = RemoteAccessMobile

SOURCES += \
    src/main.cpp \
    src/connection_manager.cpp \
    src/pcmanager.cpp \
    src/filemanager.cpp \
    src/settings_manager.cpp \
    src/pc_list_model.cpp \
    src/remote_control_client.cpp

HEADERS += \
    include/connection_manager.h \
    include/pcmanager.h \
    include/filemanager.h \
    include/settings_manager.h \
    include/pc_list_model.h \
    include/remote_control_client.h

INCLUDEPATH += include

RESOURCES += qml.qrc

QML_IMPORT_PATH =
QML_DESIGN_PATH =

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android {
    QT += androidextras
    
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    
    ANDROID_VERSION_NAME = $$VERSION
    ANDROID_VERSION_CODE = 1
    
    ANDROID_TARGET_SDK_VERSION = 33
    ANDROID_MIN_SDK_VERSION = 21
    
    DISTFILES += \
        android/AndroidManifest.xml \
        android/build.gradle \
        android/res/values/libs.xml
}
