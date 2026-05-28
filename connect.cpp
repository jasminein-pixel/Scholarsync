#include <windows.h>
#include <sqlext.h>
#include <iostream>
// 
// g++ connect.cpp -lodbc32
// -lobdc = link with library obdc
// ODBC = open database connection: translates the caller programming language to the language the remote database works on
//  Download OBDC driver "https://learn.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server"

void extractError(SQLHANDLE handle, SQLSMALLINT type);

int main()
{
    SQLHENV hEnv = NULL;   // SQLENV is a typedef for a void pointer
    SQLHDBC hDbc = NULL;   // typedef for a void pointer
    SQLHSTMT hStmt = NULL; // same, typedef to void pointer
    SQLRETURN ret;         // typedef for short int that will later hold the success, failure, warnings returned from the functions we use like 1 for sucess, 2 for sucess with wwarnins etc

    // ENV
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv); // this creates a internal structure that is like an template that will store the obdc enviroment state state and the henv pointer is sent whose points to the structure once its sent
    // SQL_HANDLE_ENV is a macro that converts to 1, the function checks the macros to know what its gonna do, like if macro is 1 that means create an enviroment struct and set the pointer to the struct
    // SQL_NULL_HANDLE is also a macro thats just 0L, thats tells the func thats this is env and is the top of hierarchy , dont attch this element to anything this IS THE STARTING POINT

    if (!SQL_SUCCEEDED(ret)) // compares the return value (that is a int) with the pre defined macros (#SQL_SUCCESS 1 etc) and returns 0 for false and 1 for true
        return 1;

    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0); // this function is used to configure the settings of obdc enviroment
    // SQL_ATTR_OBDC_VERSION is a macro, that tells the function the setting we are tryna change
    // SQL_OV_ODBC3 is also a macro, thats is the value we are changing to, so this changes the obdc version to version 3
    // we are typecasting the macro to pointer, it just tells to treat the value as pointer like if macro is defined as 3, then thats just makes it 0x0000003, its so that they dont have to write many funcs so they take all value as pointer and later convert it to int if necessary inside the function
    //  0 is the length of string, in this case we dont need to to pass a string like it could be needed if we are turining smth to like "medium" is the value we are passing to configure the setting then it would be 6

    // DBC
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc); // creates a state struct and makes hDbc point to it database state struct which is yet not connevcted but is ready to store once connected to db (database connection context object) just one live db session
    if (!SQL_SUCCEEDED(ret))                           // same as above it jus checks whether the returned thing is matching with pre defined macros or not
        return 1;

    SQLCHAR connStr[] =
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD={pass};"
        "Encrypt=yes;"
        "TrustServerCertificate=no;"
        "Connection Timeout=30;";

    // here SQLCHAR is js an typedef for datatype char and we are creating a string that holds the data or login creds about our db
    // driver = Use Microsoft’s official translator version 18 for SQL Server’s communication rules (like converting the sql query to the languiage thats native to the database server, protocol handling etc)
    //  server address is like a link, talk to this computer in internet, dns resolves that link to ip later
    //  database = whats section inside the server it gonna talk to
    //  uid = login cred ko lagi
    //  PWD = password
    //  Encrypt = azure only allows flow of encrypted data
    //  truystServerCert = no --> checks the credentials of the server before connecting to it like CA certificate etc
    //  connection timeout = 30s ma server bata reply reply aayena bhaney stop waiting else it'll wait forever

    SQLCHAR outConnStr[1024];  // SQLCHAR typedef for char, we make an empty string in which later obdc will store like the actual details sent for the connection like modify hamley mathi lekheko pwd username and fill the things hamley nalekheko and send to server
    SQLSMALLINT outConnStrLen; // mathi ko string ko length store garna, small int is typedef for short int datatype

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

    // this is the actual function tryna create the connection with the database
    // hDbc = agiko null pointer that will stores  details about the db and all once connection is established
    // NULL tells like the context keii aayo bhaney pass it to NULL aka leave it cause we dont have a gui window
    // connStr agi lekeko login cred and all details
    // SQL_NTS is a macro, that
    // outConnStr = actual modified version of Connstr thats was actually sent during connection
    // gives size of the allocated space of outconnstr  (1024 byte rn) so tells like only use this much space or overflow hunxa
    // &outConnStrLen = agi bananko int variable, yasma chahi the func will write whats the length of outConnstr thats was actually sent
    // SQL_DRIVER_NOPROMPT = yedi kunai chaiyeko kura connstr ma bhettiyena bhaney js terminate the process, dont open any prompts asking to fill this or that

    if (!SQL_SUCCEEDED(ret))
    {
        std::cout << "Connection failed\n\n";
        extractError(hDbc, SQL_HANDLE_DBC); // function to check hDbc bhitra k k aayo so we can read kei error bhayepar
        // since we are using this func to find errors query ma error bhayeni we pass SQL_HANDLE_DBC to know yo pass bhako chahi hdbc ko struct ho
        return 1;
    }

    std::cout << "Connected successfully!\n\n";

    // STMT
    // Below this is the first query sendt to the DB havent yet checked this out

    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt); // creates a structure that hold the information about the sql query details and makes hStmt point toward that structure
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

    // CLEANUP
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



// Why does order of included header file matter?
