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
    if(vac<= 0)
    {
        qDebug() << "NO MORE VACANT SPOT LEFT";
    }
    else{
    vac--;
    (*vacantSpot) = QString::number(vac);

    Notification n;
    n.createAlert(AID, db, true);

    QSqlQuery query(db);
    query.prepare("UPDATE ProjectDetails SET vacantSpot = :vs WHERE PID = :pid;");
    query.bindValue(":vs", *vacantSpot);
    query.bindValue(":pid", PID);
    if (!query.exec())
    {
        qDebug() << "Failed to update vacantSpot:" << query.lastError().text();
        return;
    }

    *status = "APPROVED";
    query.prepare("UPDATE Applications SET Status = :s WHERE AID = :aid;");
    query.bindValue(":s", *status);
    query.bindValue(":aid", AID);
    if (!query.exec())
    {
        qDebug() << "Failed to update application status:" << query.lastError().text();
    }
    }
}

void Application::reject(QSqlDatabase const &db)
{
    Notification n;
    n.createAlert(AID, db, false);

    QSqlQuery query(db);
    *status = "REJECTED";
    query.prepare("UPDATE Applications SET Status = :s WHERE AID = :aid;");
    query.bindValue(":s", *status);
    query.bindValue(":aid", AID);
    if (!query.exec())
    {
        qDebug() << "Failed to update application status:" << query.lastError().text();
    }
}

void Application::getCV()
{
    if(CV!= '0')
    {
        QDesktopServices::openUrl(QUrl(CV));
    }
    else
    {
        qDebug() << "CV UNAVAILABLE GNG";
    }
}

void Application::fillSkills(QString SID, QSqlDatabase const &db)
{
    skills.clear();
    QSqlQuery query(db);
    QString skill, proficiency, details;
    query.prepare("SELECT SkillName,Proficiency "
                  "FROM SkillList WHERE SID = :sid");
    query.bindValue(":sid", SID);

    if (query.exec())
    {
        while (query.next())
        {
            skill         = query.value("SkillName").toString();
            proficiency         = query.value("Proficiency").toString();
            details = skill + '|' + proficiency;
            skills.append(details);
        }
    }
    else
    {
        qDebug() << "failed to get skills";
    }
}

