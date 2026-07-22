#include "notification.h"
#include "modification.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

void Notification::createAlert(QString aid, QSqlDatabase const &db, bool accepted)
{
    QString pid, sid, tid, projectName, teacherName;
    QSqlQuery query(db);
    query.prepare("SELECT PID, SID FROM Applications WHERE AID = :aid");
    query.bindValue(":aid", aid);
    if (query.exec() && query.next())
    {
        pid = query.value("PID").toString();
        sid = query.value("SID").toString();
    }
    else
    {
        qDebug() << "Failed to fetch application:" << query.lastError().text();
        return;
    }

    query.prepare("SELECT ProjectName, TID FROM ProjectDetails WHERE PID = :pid");
    query.bindValue(":pid", pid);
    if (query.exec() && query.next())
    {
        projectName = query.value("ProjectName").toString();
        tid = query.value("TID").toString();
    }
    else
    {
        qDebug() << "Failed to fetch project:" << query.lastError().text();
        return;
    }

    query.prepare("SELECT Name FROM TeacherDetails WHERE TID = :tid");
    query.bindValue(":tid", tid);
    if (query.exec() && query.next())
    {
        teacherName = query.value("Name").toString();
    }
    else
    {
        qDebug() << "Failed to fetch teacher:" << query.lastError().text();
        return;
    }

    QString message;
    if (accepted)
    {
        message = "Congrats!! You Have Been Selected For %1 To Collab with %2";
        message = message.arg(projectName, teacherName);
    }
    else
    {
        message = "Sorry! You have been rejected for %1";
        message = message.arg(projectName);
    }
    QString type = "CREDIT ALERT";
    query.prepare("INSERT INTO StudentInbox (SID, Info, Type ) VALUES (:sid, :message, :type)");
    query.bindValue(":type", type);
    query.bindValue(":sid", sid);
    query.bindValue(":message", message);
    if (!query.exec())
    {
        qDebug() << "Failed to insert alert:" << query.lastError().text();
    }
}

void Notification::creditAlert(QString Credits, QString pid, QString SID, QSqlDatabase &db)
{
    QSqlQuery query(db);
    QString teacherName, projectName, tid;

    query.prepare("SELECT ProjectName, TID FROM ProjectDetails WHERE PID = :pid");
    query.bindValue(":pid", pid);
    if (query.exec() && query.next())
    {
        projectName = query.value("ProjectName").toString();
        tid = query.value("TID").toString();
    }
    else
    {
        qDebug() << "Failed to fetch project:" << query.lastError().text();
        return;
    }

    query.prepare("SELECT Name FROM TeacherDetails WHERE TID = :tid");
    query.bindValue(":tid", tid);
    if (query.exec() && query.next())
    {
        teacherName = query.value("Name").toString();
    }
    else
    {
        qDebug() << "Failed to fetch teacher:" << query.lastError().text();
        return;
    }

    QString message = "Congrats!! You Have Received %1 Credits From %2 On completion of the Project %3";
    message = message.arg(Credits, teacherName, projectName);
    QString type = "CREDIT ALERT";
    query.prepare("INSERT INTO StudentInbox (SID, Info, Type) VALUES (:sid, :message, :type)");
    query.bindValue(":sid", SID);
    query.bindValue(":message", message);
    query.bindValue(":type", type);
    if (!query.exec())
    {
        qDebug() << "Failed to insert credit alert:" << query.lastError().text();
    }
}

void Notification::extractAlerts(QString sid, QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.prepare("SELECT Info, ReceivedAt FROM StudentInbox WHERE SID = :sid");
    query.bindValue(":sid", sid);
    if (query.exec())
    {
        while (query.next())
        {
            QString message       = query.value("Info").toString();
            QString rawReceivedAt = query.value("ReceivedAt").toString();

            QString date, time;
            formatTime(rawReceivedAt, date, time);

            qDebug() << "Received At:" << date << time << "| message:" << message;
        }
    }
    else
    {
        qDebug() << "Failed to fetch alerts:" << query.lastError().text();
    }
}