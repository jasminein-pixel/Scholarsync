/*Creates an ODBC connection to Azure SQL Server usingcredentials stored in config.h and returns:
 QSqlDatabase object (opened if successful, otherwise closed with error logs)*/
#include "AzureConnection.h"

#include <QSqlError>
#include <QDebug>

QSqlDatabase AzureConnection::connect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=YOUR_PASSWORD;"
        "Encrypt=yes;"
        "TrustServerCertificate=no;"
        "Connection Timeout=30;"
    );

    if(db.open())
    {
        qDebug() << "Azure SQL Connected Successfully";
    }
    else
    {
        qDebug() << "Connection Failed:";
        qDebug() << db.lastError().text();
    }

    return db;
}