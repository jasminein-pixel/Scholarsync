#include <nanodbc/nanodbc.h>
#include <iostream>

//nanodbc is a wrapper for odbc, 

int main() {
    nanodbc::connection conn(
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=compengproj#123;"
        "Encrypt=yes;"
    );

    auto result = nanodbc::execute(conn, "SELECT TOP 5 id, name, email FROM Users");
    while (result.next()) {
        std::cout << result.get<int>(0) << " | "
                  << result.get<std::string>(1) << " | "
                  << result.get<std::string>(2) << "\n";
    }
}


//git clone https://github.com/nanodbc/nanodbc.git 
// g++ -std=c++17 connectNano.cpp nanodbc/nanodbc/nanodbc.cpp -I./nanodbc -lodbc32 -o main.exe