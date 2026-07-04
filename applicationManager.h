#pragma once
#include "fileupload.h"
#include "application.h"
#include <QVector>

class ProjApplication
{
private:
    QString PID;

    // Cached project fields populated by displayDetails()
    QString ProjectName, ProjectDesc, vacantSpot, expectedDuration,
            department, applicants, CreatedAt;

    QVector<Application> applicationList;

    void displayApplicants(QSqlDatabase &db, QString &vacantSpot);

public:
    ProjApplication(QString PID) : PID(PID) {}

    void displayDetails(QSqlDatabase &db);
};
