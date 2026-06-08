cmake_minimum_required(VERSION 3.15)
project(connectivity)

set(CMAKE_CXX_STANDARD 17)

find_package(nanodbc CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(AuthManager AuthManager.cpp)

target_link_libraries(AuthManager PRIVATE
    nanodbc
    OpenSSL::SSL
    OpenSSL::Crypto
)