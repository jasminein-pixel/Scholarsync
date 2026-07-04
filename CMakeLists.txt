cmake_minimum_required(VERSION 3.16)
project(ScholarSync VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

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
    database/DatabaseManager.h
    database/DatabaseManager.cpp
    auth/AuthManager.h
    auth/AuthManager.cpp
    models/Student.h
    models/Student.cpp
    models/Teacher.h
    models/Teacher.cpp
    windows/RegisterWindow.h
    windows/RegisterWindow.cpp
    windows/StudentDashboard.h
    windows/StudentDashboard.cpp
    windows/TeacherDashboard.h
    windows/TeacherDashboard.cpp
)

add_executable(ScholarSync ${PROJECT_SOURCES})

target_link_libraries(ScholarSync PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql)