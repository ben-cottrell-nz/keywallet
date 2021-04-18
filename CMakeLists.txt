cmake_minimum_required(VERSION 3.14)

project(keywallet LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
    set(SODIUM_DIR /home/ben/Downloads/libsodium-1.0.17-stable/libsodium-stable)
elseif (CMAKE_HOST_WIN32)
    message(STATUS CMAKE_HOST_WIN32)
    set(SODIUM_DIR C:/Users/ben/Downloads/libsodium-win64)
    set(OPENSSL_DIR C:/Qt/Tools/OpenSSL/Win_x64)
endif()

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
    set(ANDROID_SSL_PATH /home/ben/Documents/code/android_openssl)
    include(${ANDROID_SSL_PATH}/CMakeLists.txt)
    include_directories(${ANDROID_SSL_PATH}/static/include)
    message(STATUS "Android ABI is ${ANDROID_ABI}")
    list(APPEND ANDROID_EXTRA_LIBS ${SODIUM_DIR}/libsodium-android-armv7-a/lib/libsodium.so)
    if (ANDROID_ABI STREQUAL "armeabi-v7a")
        link_directories(${ANDROID_SSL_PATH}/static/lib/arm)
        link_directories(${SODIUM_DIR}/libsodium-android-armv7-a/lib)
        include_directories(${SODIUM_DIR}/libsodium-android-armv7-a/include)
    endif()
    if (ANDROID_ABI STREQUAL "arm64-v8a")
        link_directories(${ANDROID_SSL_PATH}/static/lib/arm64)
        link_directories(${SODIUM_DIR}/libsodium-android-armv8-a/lib)
        include_directories(${SODIUM_DIR}/libsodium-android-armv8-a/include)
    endif()
endif()
if (CMAKE_HOST_WIN32)
#    "c:/msys64/mingw32/include"
#            "c:/msys64/usr/include"
#            "c:/msys64/mingw32/include"

    include_directories(
        ${SODIUM_DIR}/include
        ${OPENSSL_DIR}/include
        "c:/msys64/usr/include"
        )
    link_directories("c:/msys64/mingw64/lib"
        "c:/msys32/usr/lib"
        ${SODIUM_DIR}/lib
        )
elseif (UNIX)
    link_directories(${SODIUM_DIR}/src/libsodium)
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Network Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Network Sql REQUIRED)

# to include classes to make JNI calls
if (ANDROID)
    if (${QT_VERSION_MAJOR} STREQUAL "5")
        find_package(Qt5 COMPONENTS AndroidExtras)
        include_directories(Qt5::AndroidExtras)
    endif()
endif()
include_directories(Qt${QT_VERSION_MAJOR}::Sql)
set(PROJECT_SOURCES
        main.cpp
        qml.qrc
        resources.qrc
        kw_session.cpp
        credential_store.cpp
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    if (CMAKE_HOST_WIN32)
        qt_add_executable(keywallet WIN32
            ${PROJECT_SOURCES}
        )
    else()
    qt_add_executable(keywallet
        ${PROJECT_SOURCES}
    )
    endif()
else()
    if(ANDROID)
        add_library(keywallet SHARED
            ${PROJECT_SOURCES}
        )
    else()
        if (CMAKE_HOST_WIN32)
        add_executable(keywallet WIN32
          ${PROJECT_SOURCES}
        )
        else()
            add_executable(keywallet
              ${PROJECT_SOURCES}
            )
        endif()
    endif()
endif()

if (ANDROID)
    message(STATUS ${ANDROID_EXTRA_LIBS})
    if (${QT_VERSION_MAJOR} STREQUAL "5")
        target_link_libraries(keywallet PRIVATE
            Qt5::AndroidExtras
            )
    endif()
endif()
if (CMAKE_HOST_WIN32)

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${SODIUM_DIR}/bin/libsodium-23.dll" "${CMAKE_BINARY_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E copy "${OPENSSL_DIR}/bin/libcrypto-1_1-x64.dll" "${CMAKE_BINARY_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E copy "${OPENSSL_DIR}/bin/libssl-1_1-x64.dll" "${CMAKE_BINARY_DIR}/"
        COMMENT "Copying redistributable DLL's: sodium and openssl"
    )
#    target_link_libraries(keywallet PRIVATE
#        -static-libstdc++
#        -static)
endif()
target_compile_definitions(keywallet
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(keywallet PRIVATE
  Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Sql
  sodium
  ssl
  crypto
  )
