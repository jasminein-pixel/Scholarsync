#include <QApplication>
#include <QSqlDatabase>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class Notification
{
public:
    void createAlert(QString aid, QSqlDatabase db , bool accepted);
    void creditAlert(QString Credits, QString pid, QString SID, QSqlDatabase db);
    void extractAlerts(QString sid, QSqlDatabase db);
};
