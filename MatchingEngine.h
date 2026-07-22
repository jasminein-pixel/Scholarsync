#pragma once
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include <QPair>
#include <QMap>
#include <algorithm>
#include <numeric>

class MatchingEngine {
public:
    MatchingEngine();

    // Student side → ranked list of {score, PID}
    QVector<QPair<int,int>> getRankedProjects(int SID, QSqlDatabase &db);

    // Professor side → ranked list of {score, SID}
    QVector<QPair<int,int>> getRankedStudents(int PID, QSqlDatabase &db);

    // Calculate score between one student and one project
    int calculateScore(int SID, int PID, QSqlDatabase &db);

    // Update score in Applications table
    void updateEngineScore(int SID, int PID, int score, QSqlDatabase &db);

private:
    QMap<QString, QString> synonymMap;
    QMap<QString, QString> categoryMap;

    QString normalize(const QString &skill);
    float semanticScore(const QString &a, const QString &b);
    float proficiencyFactor(int studentProf, int requiredProf);
};