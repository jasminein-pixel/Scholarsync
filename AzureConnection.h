#ifndef AZURECONNECTION_H
#define AZURECONNECTION_H

#include <QSqlDatabase>
 //This class is responsible for creating and managing the connection to the Azure SQL Database.
class AzureConnection
{
public:
 // Establishes and returns a connection to Azure SQL Database
    static QSqlDatabase connect();
};

#endif // AZURECONNECTION_H
//if you want to run and test this use main()smthg like this
/*#include <QCoreApplication>

#include "AzureConnection.h"
#include "DatabaseManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db =
        AzureConnection::connect();

    if(!db.isOpen())
    {
        return -1;
    }

    DatabaseManager manager(db);

    manager.fetchStudentSkills(1);

    manager.fetchStudentApplications(1);

    manager.fetchTeacherProjects(1);

    return a.exec();
}*/