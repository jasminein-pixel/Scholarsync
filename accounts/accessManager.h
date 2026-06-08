#pragma once
#include "auth/auth.h"
#include "database/database.h"

typedef struct Student
{
    int sid;
    std::string name;
    std::string email;
    std::string department;
    std::string program;
    std::string level;
    int semester;
    std::string preference;
    std::string contact;
}Student;

typedef struct 
{
    int tid;
    std::string name;
    std::string email;
    std::string department;
    std::string qualification;
    std::string contact;
}Teacher;

class AccountHandler
{
private:
    database &db;
    auth a;
    bool emailDoesntExist(std::string email);
    void insertStudentDetails( std::string email);
    void insertTeacherDetails( std::string email);

public:
    AccountHandler(database &db) : db(db) {};
    bool registerAccount( std::string email, bool isStudent);
    bool loginStudent( std::string email, std::string password, Student &s1);
    bool loginTeacher(std::string email, std::string password, Teacher &t1);
};
