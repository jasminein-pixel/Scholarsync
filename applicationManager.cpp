#include "applicationManager.h"

void ProjApplication::displayDetails(QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.prepare("SELECT ProjectName, ProjectDesc, vacantSpot, expectedDuration, "
                  "department, applicants, CreatedAt FROM ProjectDetails WHERE PID = :pid");
    query.bindValue(":pid", PID);

    if (query.exec() && query.next())
    {
        ProjectName       = query.value("ProjectName").toString();
        ProjectDesc       = query.value("ProjectDesc").toString();
        vacantSpot        = query.value("vacantSpot").toString();
        expectedDuration  = query.value("expectedDuration").toString();
        department        = query.value("department").toString();
        applicants        = query.value("applicants").toString();
        CreatedAt         = query.value("CreatedAt").toString();

        qDebug() << "Project:" << ProjectName << "| Desc:" << ProjectDesc
                  << "| Vacant:" << vacantSpot << "| Duration:" << expectedDuration
                  << "| Dept:" << department << "| Applicants:" << applicants
                  << "| Created:" << CreatedAt;

        // Now load the actual applicant list for this project.
        displayApplicants(db, vacantSpot);
    }
    else
    {
        qDebug() << "Error Message :" << query.lastError().text();
    }
}

void ProjApplication::displayApplicants(QSqlDatabase &db, QString &vacantSpot)
{
    applicationList.clear();

    QSqlQuery query(db);
    QString AID, SID, status, engineScore, message, appliedAt;
    query.prepare("SELECT AID, SID, Status, EngineScore, message, AppliedAt "
                  "FROM Applications WHERE PID = :pid");
    query.bindValue(":pid", PID);

    if (query.exec())
    {
        while (query.next())
        {
            AID         = query.value("AID").toString();
            SID         = query.value("SID").toString();
            status      = query.value("Status").toString();
            engineScore = query.value("EngineScore").toString();
            message     = query.value("message").toString();
            appliedAt   = query.value("AppliedAt").toString();

            // Argument order here MUST match Application's constructor:
            // (AID, vacantSpot, PID, SID, status, engineScore, message, appliedAt, db)
            // vacantSpot/status are copied by value into `temp`, so it's
            // safe for `temp` to outlive this loop iteration (no dangling
            // references, unlike the previous QString& version).
            Application temp(AID, vacantSpot, PID, SID, status, engineScore,
                              message, appliedAt, db);
            temp.display();
            temp.accept(db);
            applicationList.append(temp);
        }
    }
    else
    {
        qDebug() << "Error Message :" << query.lastError().text();
    }
}
