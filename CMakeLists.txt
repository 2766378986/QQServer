cmake_minimum_required(VERSION 3.14)
project(QtConsoleApplication VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Sql Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Sql Gui)

set(PROJECT_SOURCES
    main.cpp
    include/Server.h
    include/ConnectionPool.h
    include/ClientHandler.h
    src/Server.cpp
    src/ConnectionPool.cpp
    src/ClientHandler.cpp
)

add_executable(QtConsoleApplication ${PROJECT_SOURCES})
target_link_libraries(QtConsoleApplication Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Gui)

