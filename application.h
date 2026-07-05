#pragma once
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "notification.h"
#include "fileupload.h"
#include <QVector>


class Application
{
private:
    QString AID;
    QString *vacantSpot;
    QString PID;
    QString SID;
    QString *status;
    QString engineScore;
    QString message;
    QString appliedAt;
    QString studentName;
    QString department;
    QString program;
    QString Level;
    QString Semester;
    QString CV;
    QString Credit;
    QVector <QString> skills;
    void fillSkills(QString SID, QSqlDatabase const &db);
    void getApplicantDetails(QString SID, QSqlDatabase const &db);

public:
    Application(QString AID, QString *vacantSpot, QString PID, QString SID,
                QString *status, QString engineScore, QString message,
                QString appliedAt, QSqlDatabase &db)
        : AID(AID), vacantSpot(vacantSpot), PID(PID), SID(SID),
          status(status), engineScore(engineScore), message(message),
          appliedAt(appliedAt)
    {
        getApplicantDetails(SID, db);
        fillSkills(SID , db);
    }

    void accept(QSqlDatabase const &db);
    void reject(QSqlDatabase const &db);
    void display()
    {
        qDebug() << AID  <<" |  "<< *vacantSpot <<" |  "<< PID<<" |  "<< SID <<" |  "<< *status <<" |  "<< engineScore <<" |  "<< message <<" |  "<< appliedAt <<" |  "<< studentName<<" |  " << department <<" |  "<< program <<" |  "<< Level <<" |  "<< Semester<<" |  " <<" |  "<< Credit << "\n";
        displaySkills();
    }
    void displaySkills()
    {
        for(int i=0; i< skills.length(); i++)
        {
            qDebug() << skills[i];
        }
        qDebug() << "NEXT APPLICANTS";
        qDebug() << "\n";
    }
    void getCV();
    QString getStatus() const { return *status; }
    QString getVacantSpot() const { return *vacantSpot; }
    QString getAID() const { return AID; }
    QString getSID() const { return SID; }
    QString getStudentName() const { return studentName; }
};
