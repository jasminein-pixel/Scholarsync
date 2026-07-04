#include "AuthManager.h"
#include "../database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QRandomGenerator>
#include <QCryptographicHash>

// Register Student
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

    // Check whether email already exists
    query.prepare(
        "SELECT COUNT(*) "
        "FROM StudentDetails "
        "WHERE Email = ?"
        );

    query.addBindValue(email);

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    query.next();

    if (query.value(0).toInt() > 0)
    {
        qDebug() << "Email already exists.";
        return false;
    }

    // Generate salt
    QString salt = generateSalt();

    // Hash password
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
    query.addBindValue(0);            // For initital credit
    query.addBindValue(contactInfo);
    if (!query.exec())
    {
        qDebug() << "Registration Failed:";
        qDebug() << query.lastError().text();
        return false;
    }

    // Success
    qDebug() << "Student Registered Successfully.";

    return true;
}
bool AuthManager::loginStudent(
    const QString &email,
    const QString &password)
{
    QSqlQuery query(DatabaseManager::instance().database());

    // Find the student by email
    query.prepare(
        "SELECT PasswordHash, Salt "
        "FROM StudentDetails "
        "WHERE Email = ?"
        );

    query.addBindValue(email);

    if (!query.exec())
    {
        qDebug() << "Login query failed:";
        qDebug() << query.lastError().text();
        return false;
    }

    // No student found
    if (!query.next())
    {
        qDebug() << "Student not found.";
        return false;
    }

    // Retrieve stored credentials
    QString storedHash = query.value(0).toString();
    QString storedSalt = query.value(1).toString();

    // Verify password
    if (!verifyPassword(password, storedHash, storedSalt))
    {
        qDebug() << "Incorrect password.";
        return false;
    }

    qDebug() << "Login Successful.";

    return true;
}
//generate salt
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
//hashpassword
QString AuthManager::hashPassword(
    const QString &password,
    const QString &salt)
{
    QByteArray data = (salt + password).toUtf8();

    QByteArray hash =
        QCryptographicHash::hash(
            data,
            QCryptographicHash::Sha256
            );

    return hash.toHex();
}
//verifypassword
bool AuthManager::verifyPassword(
    const QString &password,
    const QString &storedHash,
    const QString &salt)
{
    QString enteredHash =
        hashPassword(password, salt);

    return enteredHash == storedHash;
}