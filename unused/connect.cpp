#include <windows.h>
#include <sqlext.h>
#include <iostream>

void extractError(SQLHANDLE handle, SQLSMALLINT type);

int main()
{
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;
    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (!SQL_SUCCEEDED(ret))
        return 1;

    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);

    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (!SQL_SUCCEEDED(ret))
        return 1;

    SQLCHAR connStr[] =
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD={compengproj#123};"
        "Encrypt=yes;"
        "TrustServerCertificate=no;"
        "Connection Timeout=30;";

    SQLCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;

    std::cout << "Connecting...\n";

    ret = SQLDriverConnect(
        hDbc,
        NULL,
        connStr,
        SQL_NTS,
        outConnStr,
        sizeof(outConnStr),
        &outConnStrLen,
        SQL_DRIVER_NOPROMPT);

    if (!SQL_SUCCEEDED(ret))
    {
        std::cout << "Connection failed\n\n";
        extractError(hDbc, SQL_HANDLE_DBC);
        return 1;
    }

    std::cout << "Connected successfully!\n\n";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (!SQL_SUCCEEDED(ret))
    {
        extractError(hDbc, SQL_HANDLE_DBC);
        return 1;
    }

    SQLCHAR query[] =
        "SELECT TOP 5 id, name, email, created_at FROM Users";

    ret = SQLExecDirect(hStmt, query, SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
    {
        std::cout << "Query failed\n\n";
        extractError(hStmt, SQL_HANDLE_STMT);
        return 1;
    }

    std::cout << "Top 5 Users:\n\n";

    while (SQLFetch(hStmt) == SQL_SUCCESS)
    {
        SQLINTEGER id;
        SQLCHAR name[256];
        SQLCHAR email[256];
        SQLCHAR createdAt[256];

        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, email, sizeof(email), NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, createdAt, sizeof(createdAt), NULL);

        std::cout << "ID: " << id
                  << " | Name: " << name
                  << " | Email: " << email
                  << " | Created: " << createdAt
                  << "\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    return 0;
}

void extractError(SQLHANDLE handle, SQLSMALLINT type)
{
    SQLINTEGER i = 0;
    SQLINTEGER native;
    SQLCHAR state[7];
    SQLCHAR text[256];
    SQLSMALLINT len;
    SQLRETURN ret;

    while ((ret = SQLGetDiagRec(type, handle, ++i,
                                state, &native, text,
                                sizeof(text), &len)) == SQL_SUCCESS)
    {
        std::cout << "ODBC ERROR\n";
        std::cout << "State  : " << state << "\n";
        std::cout << "Message: " << text << "\n";
        std::cout << "Native : " << native << "\n\n";
    }
}
