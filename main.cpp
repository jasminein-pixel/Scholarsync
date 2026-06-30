
#include <QSqlDatabase>
#include "fileupload.h"
#include "notification.h"
#include <QSqlError>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString name = "Rahul";
    File f1;
    // f1.uploadCV("1" , name);
    // // f1.downloadCV("1" , name);
    // qDebug() << QSqlDatabase::drivers();
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
        // f1.uploadCV("1" , name, db);

        Notification n1;
        n1.creditAlert("10", "1", "1", db);
        // n1.createAlert("1", db , 0);
        // n1.extractAlerts("1", db);
    }
    else
    {
        qDebug() << "Connection Failed:";
        qDebug() << db.lastError().text();
    }

    return app.exec();
}