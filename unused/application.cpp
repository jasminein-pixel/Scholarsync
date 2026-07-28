// ARCHIVED — not compiled. See unused/application.h for notes.
#include "application.h"

void Application::getApplicantDetails(QString SID, QSqlDatabase const &db)
{
    QSqlQuery query(db);
    query.prepare("SELECT Name, Department, Program, Level, Semester, cv_url, Credit "
                  "FROM StudentDetails WHERE SID = :sid");
    query.bindValue(":sid", SID);

    if (query.exec() && query.next())
    {
        studentName = query.value("Name").toString();
        department  = query.value("Department").toString();
        program     = query.value("Program").toString();
        Level       = query.value("Level").toString();
        Semester    = query.value("Semester").toString();
        CV          = query.value("cv_url").toString();
        Credit      = query.value("Credit").toString();
    }
    else
    {
        qDebug() << "Failed to fetch applicant details:" << query.lastError().text();
    }
}

void Application::accept(QSqlDatabase const &db)
{
    qint16 vac = (*vacantSpot).toInt();
    if (vac <= 0) { qDebug() << "NO MORE VACANT SPOT LEFT"; return; }
    vac--;
    (*vacantSpot) = QString::number(vac);

    // Notification n; n.createAlert(AID, db, true); // requires Notification class
    QSqlQuery query(db);
    query.prepare("UPDATE ProjectDetails SET vacantSpot = :vs WHERE PID = :pid;");
    query.bindValue(":vs", *vacantSpot);
    query.bindValue(":pid", PID);
    query.exec();

    *status = "APPROVED";
    query.prepare("UPDATE Applications SET Status = :s WHERE AID = :aid;");
    query.bindValue(":s", *status);
    query.bindValue(":aid", AID);
    query.exec();
}

void Application::reject(QSqlDatabase const &db)
{
    QSqlQuery query(db);
    *status = "REJECTED";
    query.prepare("UPDATE Applications SET Status = :s WHERE AID = :aid;");
    query.bindValue(":s", *status);
    query.bindValue(":aid", AID);
    query.exec();
}

void Application::getCV()
{
    if (CV != '0')
        QDesktopServices::openUrl(QUrl(CV));
    else
        qDebug() << "CV UNAVAILABLE";
}

void Application::fillSkills(QString SID, QSqlDatabase const &db)
{
    skills.clear();
    QSqlQuery query(db);
    query.prepare("SELECT SkillName, Proficiency FROM SkillList WHERE SID = :sid");
    query.bindValue(":sid", SID);
    if (query.exec())
    {
        while (query.next())
        {
            QString skill = query.value("SkillName").toString();
            QString prof  = query.value("Proficiency").toString();
            skills.append(skill + "|" + prof);
        }
    }
    else { qDebug() << "failed to get skills"; }
}
