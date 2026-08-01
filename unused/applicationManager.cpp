// ARCHIVED — not compiled. See unused/application.h for notes.
#include "applicationManager.h"

void ProjApplication::displayDetails(QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.prepare("SELECT ProjectName, ProjectDesc, vacantSpot, expectedDuration, "
                  "department, applicants, CreatedAt FROM ProjectDetails WHERE PID = :pid");
    query.bindValue(":pid", PID);

    if (query.exec() && query.next())
    {
        ProjectName      = query.value("ProjectName").toString();
        ProjectDesc      = query.value("ProjectDesc").toString();
        vacantSpot       = query.value("vacantSpot").toString();
        expectedDuration = query.value("expectedDuration").toString();
        department       = query.value("department").toString();
        applicants       = query.value("applicants").toString();
        CreatedAt        = query.value("CreatedAt").toString();

        qDebug() << "Project:" << ProjectName << "| Desc:" << ProjectDesc
                 << "| Vacant:" << vacantSpot << "| Duration:" << expectedDuration
                 << "| Dept:" << department << "| Applicants:" << applicants
                 << "| Created:" << CreatedAt << "\n";
        displayApplicants(db, vacantSpot);
    }
    else { qDebug() << "Error:" << query.lastError().text(); }
}

void ProjApplication::displayApplicants(QSqlDatabase &db, QString &vacantSpot)
{
    applicationList.clear();
    QSqlQuery query(db);
    query.prepare("SELECT AID, SID, Status, EngineScore, message, AppliedAt "
                  "FROM Applications WHERE PID = :pid");
    query.bindValue(":pid", PID);

    if (query.exec())
    {
        while (query.next())
        {
            QString AID         = query.value("AID").toString();
            QString SID         = query.value("SID").toString();
            QString status      = query.value("Status").toString();
            QString engineScore = query.value("EngineScore").toString();
            QString message     = query.value("message").toString();
            QString appliedAt   = query.value("AppliedAt").toString();

            Application temp(AID, &vacantSpot, PID, SID, &status, engineScore,
                             message, appliedAt, db);
            temp.display();
            temp.getCV();
            applicationList.append(temp);
        }
    }
    else { qDebug() << "Error:" << query.lastError().text(); }
}
