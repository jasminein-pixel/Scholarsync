#pragma once
#include <nanodbc/nanodbc.h>
#include <iostream>

class database
{
private:
    // this line initializes the object connection and also creates actual connection with db
    nanodbc::connection databaseInfo{
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=engineering$123;"
        "Encrypt=yes;"
        "Connection Timeout=30;"
        "MARS_Connection=yes;"};

public:
    nanodbc::result database::sendQuery(nanodbc::statement query);
    nanodbc::statement queryPrep(std::string checkQuery);
    void ErrorDisplay(const nanodbc::database_error &e);
};
