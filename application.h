#pragma once
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "notification.h"

class Application
{
private:
    QString AID;
    QString vacantSpot;
    QString PID;
    QString SID;
    QString status;
    QString engineScore;
    QString message;
    QString appliedAt;
    // Fetched from StudentDetails
    QString studentName;
    QString department;
    QString program;
    QString Level;
    QString Semester;
    QString CV;
    QString Credit;

    void getApplicantDetails(QString SID, QSqlDatabase const &db);

public:
    // NOTE: parameter order below must match how callers construct this object.
    //
    // vacantSpot/status used to be QString& members. That's dangerous: any
    // Application stored in a container (e.g. QVector<Application>) would
    // end up holding a reference to whatever local variable the caller
    // passed in, which is often destroyed before accept()/reject() is ever
    // called -> dangling reference / undefined behavior. They are now
    // owned values; use getVacantSpot()/getStatus() after calling
    // accept()/reject() if the caller needs the updated value.
    Application(QString AID, QString vacantSpot, QString PID, QString SID,
                QString status, QString engineScore, QString message,
                QString appliedAt, QSqlDatabase &db)
        : AID(AID), vacantSpot(vacantSpot), PID(PID), SID(SID),
          status(status), engineScore(engineScore), message(message),
          appliedAt(appliedAt)
    {
        getApplicantDetails(SID, db);
    }

    void accept(QSqlDatabase const &db);
    void reject(QSqlDatabase const &db);
    void display()
    {
        qDebug() << AID  <<" |  "<< vacantSpot <<" |  "<< PID<<" |  "<< SID <<" |  "<< status <<" |  "<< engineScore <<" |  "<< message <<" |  "<< appliedAt <<" |  "<< studentName<<" |  " << department <<" |  "<< program <<" |  "<< Level <<" |  "<< Semester<<" |  " << CV <<" |  "<< Credit << "\n";

    }
    QString getStatus() const { return status; }
    QString getVacantSpot() const { return vacantSpot; }
    QString getAID() const { return AID; }
    QString getSID() const { return SID; }
    QString getStudentName() const { return studentName; }
};
