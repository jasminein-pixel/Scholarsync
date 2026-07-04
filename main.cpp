
#include <QSqlDatabase>
#include <QSqlError>
#include "applicationManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    File f1;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=;"
        "Encrypt=yes;"
        "TrustServerCertificate=no;"
        "Connection Timeout=30;");

    if (db.open())
    {
        qDebug() << "Azure SQL Connected Successfully";
        ProjApplication p("1");
        p.displayDetails(db);
    }
    else
    {
        qDebug() << "Connection Failed:";
        qDebug() << db.lastError().text();
    }

    return app.exec();
}