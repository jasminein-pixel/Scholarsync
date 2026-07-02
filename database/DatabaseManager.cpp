#include "DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
}

bool DatabaseManager::connect()
{
    if (QSqlDatabase::contains("qt_conn"))
    {
        db = QSqlDatabase::database("qt_conn");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QODBC", "qt_conn");
    }
    QString connStr =
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net,1433;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=secretpassword123#;"
        "Encrypt=yes;"
        "TrustServerCertificate=yes;"
        "Connection Timeout=30;";
    db.setDatabaseName(connStr);
    qDebug() << "Attempting connection...";
    if (!db.open())
    {
        qDebug() << "FAILED:" << db.lastError().text();
        connected = false;
        return false;
    }
    qDebug() << "CONNECTED SUCCESSFULLY";
    connected = true;
    return true;
}

QSqlQuery DatabaseManager::executeQuery(const QString &query)
{
    QSqlQuery q(db);
    if (!isConnected())
    {
        qDebug() << "❌ Database not connected!";
        return q;
    }
    if (!q.exec(query))
    {
        qDebug() << "❌ Query failed:";
        qDebug() << q.lastError().text();
    }
    return q;
}

QSqlQuery DatabaseManager::prepareAndExecute(const QString &queryStr, const QVariantList &params)
{
    QSqlQuery q(db);
    if (!isConnected())
    {
        qDebug() << "❌ Database not connected!";
        return q;
    }
    q.prepare(queryStr);
    for (const QVariant &p : params)
        q.addBindValue(p);
    if (!q.exec())
    {
        qDebug() << "❌ Prepared query failed:";
        qDebug() << q.lastError().text();
    }
    return q;
}

bool DatabaseManager::isConnected() const
{
    return connected && db.isOpen();
}
QSqlDatabase DatabaseManager::database()
{
    return db;
}