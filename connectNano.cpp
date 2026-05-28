#include <nanodbc/nanodbc.h>
#include <iostream>

//nanodbc is a wrapper for odbc, 
//check connect.cpp for how nanodbc works deep down

// Running The Program :
// ODBC = open database connection: translates the caller programming language to the language the remote database works on
//  Download OBDC driver "https://learn.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server"
//  git clone https://github.com/nanodbc/nanodbc.git 
//  g++ -std=c++17 connectNano.cpp nanodbc/nanodbc/nanodbc.cpp -I./nanodbc -lodbc32 -o main.exe


int main() {
    nanodbc::connection conn(
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=passwordhere;"
        "Encrypt=yes;"
    );

    auto result = nanodbc::execute(conn, "SELECT TOP 5 id, name, email FROM Users");
    while (result.next()) {
        std::cout << result.get<int>(0) << " | "
                  << result.get<std::string>(1) << " | "
                  << result.get<std::string>(2) << "\n";
    }
}


