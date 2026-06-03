#include <nanodbc/nanodbc.h>
#include <iostream>

// nanodbc is a wrapper for odbc,
// check connect.cpp for how nanodbc works deep down
//  Running The Program :
//  ODBC = open database connection: translates the caller programming language to the language the remote database works on
//  Download OBDC driver "https://learn.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server"
//  git clone https://github.com/nanodbc/nanodbc.git
//  g++ -std=c++17 connectNano.cpp nanodbc/nanodbc/nanodbc.cpp -I./nanodbc -lodbc32 -o main.exe

int main()
{
    try
    {
        nanodbc::connection conn(
            "DRIVER={ODBC Driver 18 for SQL Server};"
            "SERVER=scholarsync.database.windows.net;"
            "DATABASE=ScholarSync;"
            "UID=scholarsync;"
            "PWD=;"
            "Encrypt=yes;"
            "Connection Timeout=30");

        std::string query = "SELECT TOP (10) * FROM [dbo].[StudentDetails]";
        std::cout << std::endl;
        std::cout << "------------STUDENT DETAILS------------\n";
        auto result = nanodbc::execute(conn, query);
        while (result.next())
        {
            std::cout << "s.id : " << result.get<int>(0) << "\n"
                      << "Name : " << result.get<std::string>(1) << "\n"
                      << "Email : " << result.get<std::string>(2) << "\n"
                      << "Password : " << result.get<std::string>(3) << "\n"
                      << "Department : " << result.get<std::string>(4) << "\n"
                      << "Program : " << result.get<std::string>(5) << "\n"
                      << "Level : " << result.get<std::string>(6) << "\n"
                      << "Semester : " << result.get<int>(7) << "\n"
                      << "Preference : " << result.get<std::string>(8) << "\n"
                      << "PhoneNum : " << result.get<std::string>(9) << "\n"
                      << std::endl;
        }

        std::cout << std::endl;
        std::cout << "------------TEACHER DETAILS------------\n";

        query = "SELECT TOP (10) * FROM [dbo].[TeacherDetails]";
        result = nanodbc::execute(conn, query);
        while (result.next())
        {
            std::cout << "t.id : " << result.get<int>(0) << "\n"
                      << "Name : " << result.get<std::string>(1) << "\n"
                      << "Email : " << result.get<std::string>(2) << "\n"
                      << "Password : " << result.get<std::string>(3) << "\n"
                      << "Department : " << result.get<std::string>(4) << "\n"
                      << "Qualification : " << result.get<std::string>(5) << "\n"
                      << "PhoneNum : " << result.get<std::string>(6) << "\n"
                      << std::endl;
        }
    }

    catch (const nanodbc::database_error &e)
    {
        std::cout << "===== DATABASE ERROR =====\n";
        std::cout << "Message : " << e.what() << "\n";
        std::cout << "Native  : " << e.native() << "\n";
        switch (e.native())
        {
        case 0:
            std::cout << "0 = OBDC didnt run";
            break;
        case 53:
            std::cout << "Server not found";
            break;
        case 258:
            std::cout << "Maybe Wrong UID or Server Offline (Keep trying it'll resume the server)";
            break;
        case 18456:
            std::cout << "Server Found, Couldnt login Check DatabaseName / UID / passwrd ";
            break;
        }
    }
}
