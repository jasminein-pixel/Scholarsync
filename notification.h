#pragma once
#include <QString>
#include <QSqlDatabase>

class Notification
{
public:
    void createAlert(QString aid, QSqlDatabase const &db, bool accepted);
    void creditAlert(QString Credits, QString pid, QString SID, QSqlDatabase &db);
    void extractAlerts(QString sid, QSqlDatabase &db);
};
