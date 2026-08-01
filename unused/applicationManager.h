// ARCHIVED — not compiled. See unused/application.h for notes.
#pragma once
#include "application.h"
#include <QVector>

class ProjApplication
{
private:
    QString PID;
    QString ProjectName, ProjectDesc, vacantSpot, expectedDuration,
            department, applicants, CreatedAt;
    QVector<Application> applicationList;
    void displayApplicants(QSqlDatabase &db, QString &vacantSpot);

public:
    ProjApplication(QString PID) : PID(PID) {}
    void displayDetails(QSqlDatabase &db);
};
