cmake_minimum_required(VERSION 3.16)
project(ScholarSync)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql)

add_executable(ScholarSync main.cpp
    fileupload.cpp
    fileupload.h
    notification.h
    notification.cpp
)

target_link_libraries(ScholarSync PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
)