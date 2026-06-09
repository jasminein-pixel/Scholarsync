/*DatabaseManager.cpp is where all the database work actually happens. It runs SQL queries to get data like student skills, applications, and teacher projects from the database. It takes the results and prints them in the console. Basically, this file connects the app to the database and helps fetch and show the needed information in a simple way.*/
#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DatabaseManager::DatabaseManager(QSqlDatabase database)
{
    db = database;
}

void DatabaseManager::fetchStudentSkills(int sid)
{
    QSqlQuery query(db);

    query.prepare(
        "SELECT SkillName, Proficiency "
        "FROM SkillList "
        "WHERE SID = :sid"
    );

    query.bindValue(":sid", sid);

    if(query.exec())
    {
        qDebug() << "\n===== STUDENT SKILLS =====";

        while(query.next())
        {
            QString skill =
                query.value("SkillName").toString();

            QString proficiency =
                query.value("Proficiency").toString();

            qDebug()
                << "Skill:"
                << skill
                << "| Proficiency:"
                << proficiency;
        }
    }
    else
    {
        qDebug() << "Skill Query Failed:";
        qDebug() << query.lastError().text();
    }
}

void DatabaseManager::fetchStudentApplications(int sid)
{
    QSqlQuery query(db);

    query.prepare(R"(

        SELECT
            p.ProjectName,
            a.Status,
            a.EngineScore,
            a.message,
            a.AppliedAt

        FROM Applications a

        INNER JOIN ProjectDetails p
        ON a.PID = p.PID

        WHERE a.SID = :sid

        ORDER BY a.AppliedAt DESC

    )");

    query.bindValue(":sid", sid);

    if(query.exec())
    {
        qDebug() << "\n===== STUDENT APPLICATIONS =====";

        while(query.next())
        {
            QString projectName =
                query.value("ProjectName").toString();

            QString status =
                query.value("Status").toString();

            QString score =
                query.value("EngineScore").toString();

            QString message =
                query.value("message").toString();

            QString appliedAt =
                query.value("AppliedAt").toString();

            qDebug()
                << "\nProject:"
                << projectName;

            qDebug()
                << "Status:"
                << status;

            qDebug()
                << "Engine Score:"
                << score;

            qDebug()
                << "Message:"
                << message;

            qDebug()
                << "Applied At:"
                << appliedAt;
        }
    }
    else
    {
        qDebug() << "Application Query Failed:";
        qDebug() << query.lastError().text();
    }
}

void DatabaseManager::fetchTeacherProjects(int tid)
{
    QSqlQuery query(db);

    query.prepare(R"(

        SELECT
            PID,
            ProjectName,
            ProjectDesc,
            VacantSpot,
            ExpectedDuration,
            Department,
            Preference,
            Applications,
            Status,
            CreatedAt

        FROM ProjectDetails

        WHERE TID = :tid

        ORDER BY CreatedAt DESC

    )");

    query.bindValue(":tid", tid);

    if(query.exec())
    {
        qDebug() << "\n===== TEACHER PROJECTS =====";

        while(query.next())
        {
            qDebug()
                << "\nProject:"
                << query.value("ProjectName").toString();

            qDebug()
                << "Description:"
                << query.value("ProjectDesc").toString();

            qDebug()
                << "Vacant Spots:"
                << query.value("VacantSpot").toString();

            qDebug()
                << "Expected Duration:"
                << query.value("ExpectedDuration").toString();

            qDebug()
                << "Department:"
                << query.value("Department").toString();

            qDebug()
                << "Preference:"
                << query.value("Preference").toString();

            qDebug()
                << "Applications:"
                << query.value("Applications").toString();

            qDebug()
                << "Status:"
                << query.value("Status").toString();

            qDebug()
                << "Created At:"
                << query.value("CreatedAt").toString();
        }
    }
    else
    {
        qDebug() << "Teacher Project Query Failed:";
        qDebug() << query.lastError().text();
    }
}