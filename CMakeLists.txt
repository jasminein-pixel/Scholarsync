cmake_minimum_required(VERSION 3.16)

project(ScholarSync VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    student_login.h
    student_login.cpp
    student_login.ui
    teacher_login.h
    teacher_login.cpp
    teacher_login.ui
)

add_executable(ScholarSync ${PROJECT_SOURCES})

target_link_libraries(ScholarSync PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# If/when you wire up nanodbc + Azure SQL, link it here, e.g.:
# find_package(nanodbc REQUIRED)
# target_link_libraries(ScholarSync PRIVATE nanodbc::nanodbc)