#include "accessManager.h"
bool AccountHandler::emailDoesntExist(std::string email)
{
    std::string checkQuery = "SELECT COUNT(*) FROM StudentDetails WHERE Email = ?";
    auto stmt = db.queryPrep(checkQuery);
    stmt.bind(0, email.c_str());
    auto check = db.sendQuery(stmt);
    check.next();
    if (check.get<int>(0) > 0)
    {
        std::cout << "Email already registered!\n";
        return false;
    }
    else
    {
        return true;
    }
}

void AccountHandler::insertStudentDetails(std::string email)
{
    std::string name, department, program, level, preference, contact;
    int semester;
    std::cin >> name >> department >> program >> level >> semester >> preference >> contact;
    std::string query =
        "UPDATE StudentDetails "
        "SET Name = ?, Department = ?, Program = ?, level = ? , Semester = ? , preference = ? , contact = ? "
        "WHERE Email = ?";
    auto insertStmt = db.queryPrep(query);
    insertStmt.bind(0, name.c_str());
    insertStmt.bind(1, department.c_str());
    insertStmt.bind(2, program.c_str());
    insertStmt.bind(3, level.c_str());
    insertStmt.bind(4, &semester);
    insertStmt.bind(5, preference.c_str());
    insertStmt.bind(6, contact.c_str());
    insertStmt.bind(7, email.c_str());
    db.sendQuery(insertStmt);
}

void AccountHandler::insertTeacherDetails(std::string email)
{
    std::string name, department, qualification, contact;
    std::cin >> name >> department >> qualification >> contact;
    std::string query =
        "UPDATE TeacherDetails SET Name = ?, Department = ?, Qualification = ?, ContactInfo = ? WHERE Email = ?";
    auto insertStmt = db.queryPrep(query);
    insertStmt.bind(0, name.c_str());
    insertStmt.bind(1, department.c_str());
    insertStmt.bind(2, qualification.c_str());
    insertStmt.bind(3, contact.c_str());
    insertStmt.bind(4, email.c_str());
    db.sendQuery(insertStmt);
}

bool AccountHandler::registerAccount(std::string email, bool isStudent)
{
    std::string password, name, department, program, level, semester, preference, contact, tableName;
    if (emailDoesntExist(email))
    {
        std::cin >> password;
        std::string hashed = a.hashPassword(password);
        if (isStudent)
            tableName = "studentDetails";
        else
            tableName = "teacherDetails";
        std::string query =
            "INSERT INTO " + tableName +
            " (Email, PasswordHash) "
            "VALUES (?, ?)";

        auto insertStmt = db.queryPrep(query);
        insertStmt.bind(0, email.c_str());
        insertStmt.bind(1, hashed.c_str());
        db.sendQuery(insertStmt);
        if (isStudent)
        {
            insertStudentDetails(email);
            std::cout << "Student registered successfully!\n";
        }
        else
        {
            insertTeacherDetails(email);
            std::cout << "Teacher registered successfully!\n";
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool AccountHandler::loginStudent(std::string email, std::string password, Student &s1)
{
    std::string query =
        "SELECT PasswordHash FROM StudentDetails WHERE Email = ?";
    auto stmt = db.queryPrep(query);
    stmt.bind(0, email.c_str());
    auto result = db.sendQuery(stmt);
    if (!result.next())
    {
        std::cout << "No student found with that email.\n";
        return false;
    }
    std::string storedHash = result.get<std::string>(0);

    // Verify password
    if (a.verifyPassword(password, storedHash))
    {
        std::cout << "Login successful!\n";
        query =
            "SELECT  SID, Name, Department, Program, Level, Semester, Preference, ContactInfo FROM StudentDetails WHERE Email = ?";
        auto stmt = db.queryPrep(query);
        stmt.bind(0, email.c_str());
        auto result = db.sendQuery(stmt);
        s1.sid = result.get<int>(0);
        s1.name = result.get<std::string>(1);
        s1.department = result.get<std::string>(2);
        s1.program = result.get<std::string>(3);
        s1.level = result.get<std::string>(4);
        s1.semester = result.get<int>(5);
        s1.preference = result.get<std::string>(6);
        s1.contact = result.get<std::string>(7);
        return true;
    }
    else
    {
        std::cout << "Incorrect password.\n";
        return false;
    }
};

bool AccountHandler::loginTeacher(std::string email, std::string password, Teacher &t1)
{
    std::string query =
        "SELECT PasswordHash FROM TeacherDetails WHERE Email = ?";
    auto stmt = db.queryPrep(query);
    stmt.bind(0, email.c_str());
    auto result = db.sendQuery(stmt);
    if (!result.next())
    {
        std::cout << "No Teacher found with that email.\n";
        return false;
    }
    std::string storedHash = result.get<std::string>(0);

    // Verify password
    if (a.verifyPassword(password, storedHash))
    {
        std::cout << "Login successful!\n";
        query =
            "SELECT  TID, Name, Department, Qualification , ContactInfo FROM TeacherDetails WHERE Email = ?";
        auto stmt = db.queryPrep(query);
        stmt.bind(0, email.c_str());
        auto result = db.sendQuery(stmt);
        t1.tid = result.get<int>(0);
        t1.name = result.get<std::string>(1);
        t1.department = result.get<std::string>(2);
        t1.qualification = result.get<std::string>(3);
        t1.contact = result.get<std::string>(4);
        return true;
    }
    else
    {
        std::cout << "Incorrect password.\n";
        return false;
    }
}