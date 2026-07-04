#include "AuthManager.h"
#include "../database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QRandomGenerator>
#include <QCryptographicHash>

// ─────────────────────────────────────────
//  Password Utilities
// ─────────────────────────────────────────

QString AuthManager::generateSalt()
{
    QByteArray salt;
    for (int i = 0; i < 16; i++)
    {
        salt.append(
            static_cast<char>(
                QRandomGenerator::global()->bounded(256)
                )
            );
    }
    return salt.toHex();
}

QString AuthManager::hashPassword(
    const QString &password,
    const QString &salt)
{
    QByteArray data = (salt + password).toUtf8();
    QByteArray hash = QCryptographicHash::hash(
        data,
        QCryptographicHash::Sha256
        );
    return hash.toHex();
}

bool AuthManager::verifyPassword(
    const QString &password,
    const QString &storedHash,
    const QString &salt)
{
    return hashPassword(password, salt) == storedHash;
}

// ─────────────────────────────────────────
//  Student Authentication
// ─────────────────────────────────────────

bool AuthManager::registerStudent(
    const QString &name,
    const QString &email,
    const QString &password,
    const QString &department,
    const QString &program,
    const QString &level,
    int semester,
    const QString &preference,
    const QString &contactInfo)
{
    QSqlQuery query(DatabaseManager::instance().database());

    // Check if email already exists
    query.prepare(
        "SELECT COUNT(*) FROM StudentDetails WHERE Email = ?"
        );
    query.addBindValue(email);

    if (!query.exec())
    {
        qDebug() << "Check failed:" << query.lastError().text();
        return false;
    }

    query.next();
    if (query.value(0).toInt() > 0)
    {
        qDebug() << "Email already exists.";
        return false;
    }

    QString salt           = generateSalt();
    QString hashedPassword = hashPassword(password, salt);

    query.prepare(
        "INSERT INTO StudentDetails "
        "(Name, Email, PasswordHash, Salt, Department, Program, "
        "Level, Semester, Preference, Credit, ContactInfo) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );

    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(hashedPassword);
    query.addBindValue(salt);
    query.addBindValue(department);
    query.addBindValue(program);
    query.addBindValue(level);
    query.addBindValue(semester);
    query.addBindValue(preference);
    query.addBindValue(0);
    query.addBindValue(contactInfo);

    if (!query.exec())
    {
        qDebug() << "Student Registration Failed:" << query.lastError().text();
        return false;
    }

    qDebug() << "Student Registered Successfully.";
    return true;
}

bool AuthManager::loginStudent(
    const QString &email,
    const QString &password)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(
        "SELECT PasswordHash, Salt FROM StudentDetails WHERE Email = ?"
        );
    query.addBindValue(email);

    if (!query.exec())
    {
        qDebug() << "Login query failed:" << query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        qDebug() << "Student not found.";
        return false;
    }

    QString storedHash = query.value(0).toString();
    QString storedSalt = query.value(1).toString();

    if (!verifyPassword(password, storedHash, storedSalt))
    {
        qDebug() << "Incorrect password.";
        return false;
    }

    qDebug() << "Student Login Successful.";
    return true;
}

// ─────────────────────────────────────────
//  Teacher Authentication
// ─────────────────────────────────────────

bool AuthManager::registerTeacher(
    const QString &name,
    const QString &email,
    const QString &password,
    const QString &department,
    const QString &qualification,
    const QString &contactInfo)
{
    QSqlQuery query(DatabaseManager::instance().database());

    // Check if email already exists
    query.prepare(
        "SELECT COUNT(*) FROM TeacherDetails WHERE Email = ?"
        );
    query.addBindValue(email);

    if (!query.exec())
    {
        qDebug() << "Check failed:" << query.lastError().text();
        return false;
    }

    query.next();
    if (query.value(0).toInt() > 0)
    {
        qDebug() << "Email already exists.";
        return false;
    }

    QString salt           = generateSalt();
    QString hashedPassword = hashPassword(password, salt);

    query.prepare(
        "INSERT INTO TeacherDetails "
        "(Name, Email, PasswordHash, Salt, Department, Qualification, ContactInfo) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
        );

    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(hashedPassword);
    query.addBindValue(salt);
    query.addBindValue(department);
    query.addBindValue(qualification);
    query.addBindValue(contactInfo);

    if (!query.exec())
    {
        qDebug() << "Teacher Registration Failed:" << query.lastError().text();
        return false;
    }

    qDebug() << "Teacher Registered Successfully.";
    return true;
}

bool AuthManager::loginTeacher(
    const QString &email,
    const QString &password)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(
        "SELECT PasswordHash, Salt FROM TeacherDetails WHERE Email = ?"
        );
    query.addBindValue(email);

    if (!query.exec())
    {
        qDebug() << "Login query failed:" << query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        qDebug() << "Teacher not found.";
        return false;
    }

    QString storedHash = query.value(0).toString();
    QString storedSalt = query.value(1).toString();

    if (!verifyPassword(password, storedHash, storedSalt))
    {
        qDebug() << "Incorrect password.";
        return false;
    }

    qDebug() << "Teacher Login Successful.";
    return true;
}