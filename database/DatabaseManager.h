#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
QSqlDatabase database();
class DatabaseManager
{
public:
    static DatabaseManager& instance();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool connect();
    bool isConnected() const;
    QSqlDatabase database();
    QSqlQuery executeQuery(const QString &query);
    QSqlQuery prepareAndExecute(const QString &queryStr, const QVariantList &params);

private:
    DatabaseManager();
    QSqlDatabase db;
    bool connected = false;
};

#endif