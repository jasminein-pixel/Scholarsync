cmake_minimum_required(VERSION 3.16)
project(ScholarSync VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui

    views/student_login.h
    views/student_login.cpp
    views/student_login.ui
    views/teacher_login.h
    views/teacher_login.cpp
    views/teacher_login.ui
    views/forgotpassword.h
    views/forgotpassword.cpp

    database/DatabaseManager.h
    database/DatabaseManager.cpp
    auth/AuthManager.h
    auth/AuthManager.cpp
    models/Student.h
    models/Student.cpp
    models/Teacher.h
    models/Teacher.cpp

    services/notification.h
    services/notification.cpp
    services/fileupload.h
    services/fileupload.cpp
    services/modification.h
    services/modification.cpp
    services/MatchingEngine.h
    services/MatchingEngine.cpp

    windows/RegisterWindow.h
    windows/RegisterWindow.cpp

    windows/student/StudentDashboard.h
    windows/student/StudentDashboard.cpp
    windows/student/StudentStyles.h
    windows/student/PieChartWidget.h
    windows/student/PieChartWidget.cpp
    windows/student/StudentSkillsScreen.cpp
    windows/student/StudentInterestsScreen.cpp
    windows/student/StudentBrowseScreen.cpp
    windows/student/StudentCVScreen.cpp
    windows/student/StudentRecommendScreen.cpp
    windows/student/StudentInboxScreen.cpp
    windows/student/StudentProfileScreen.cpp

    windows/teacher/TeacherDashboard.h
    windows/teacher/TeacherDashboard.cpp
    windows/teacher/TeacherStyles.h
    windows/teacher/TeacherProjectsScreen.cpp
    windows/teacher/TeacherPostScreen.cpp
    windows/teacher/TeacherApplicantsScreen.cpp
)

add_executable(ScholarSync ${PROJECT_SOURCES})

target_link_libraries(ScholarSync PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network)