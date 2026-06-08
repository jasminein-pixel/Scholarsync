#include "database.h"

nanodbc::result sendQuery(nanodbc::statement query)
{
    return nanodbc::execute(query);
}

nanodbc::statement database::queryPrep(std::string checkQuery)
{
    return nanodbc::statement(databaseInfo, checkQuery);
}

void database::ErrorDisplay(const nanodbc::database_error &e)
{
    std::cout << "--------------DATABASE ERROR--------------\n";
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