/*
 * DatabaseManager
 * ----------------
 * Handles all database-related queries for the application.
 *
 * This class performs SELECT operations such as:
 * - Fetching student skills
 * - Fetching student applications
 * - Fetching teacher projects
 */
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
private:
    QSqlDatabase db;

public:
    DatabaseManager(QSqlDatabase database);

    void fetchStudentSkills(int sid);

    void fetchStudentApplications(int sid);

    void fetchTeacherProjects(int tid);
};

#endif // DATABASEMANAGER_H