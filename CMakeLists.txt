cmake_minimum_required(VERSION 3.16)

project(QtAzureConnection VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Sql
    Widgets
)

qt_standard_project_setup()

qt_add_executable(QtAzureConnection
    main.cpp

    database/DatabaseManager.h
    database/DatabaseManager.cpp

    models/Student.h
    models/Student.cpp
    models/Teacher.h
    models/Teacher.cpp

    auth/AuthManager.h
    auth/AuthManager.cpp

    windows/LoginWindow.h
    windows/LoginWindow.cpp
    windows/LoginWindow.ui

    windows/RegisterWindow.h
    windows/RegisterWindow.cpp
    windows/RegisterWindow.ui

    windows/StudentDashboard.h
    windows/StudentDashboard.cpp
    windows/StudentDashboard.ui

    windows/TeacherDashboard.h
    windows/TeacherDashboard.cpp
    windows/TeacherDashboard.ui
)

target_link_libraries(QtAzureConnection PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Widgets
)