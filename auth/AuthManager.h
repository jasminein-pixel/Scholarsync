#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>

class AuthManager
{
public:

    // ----------------------------
    // Password Utilities
    // ----------------------------

    static QString generateSalt();

    static QString hashPassword(const QString &password,
                                const QString &salt);

    static bool verifyPassword(const QString &password,
                               const QString &storedHash,
                               const QString &salt);

    // ----------------------------
    // Student Authentication
    // ----------------------------

    static bool registerStudent(const QString &name,
                                const QString &email,
                                const QString &password,
                                const QString &department,
                                const QString &program,
                                const QString &level,
                                int semester,
                                const QString &preference,
                                const QString &contactInfo);

    static bool loginStudent(const QString &email,
                             const QString &password);

    // ----------------------------
    // Teacher Authentication
    // ----------------------------

    static bool registerTeacher(const QString &name,
                                const QString &email,
                                const QString &password,
                                const QString &department,
                                const QString &qualification,
                                const QString &contactInfo);

    static bool loginTeacher(const QString &email,
                             const QString &password);
};

#endif
//authmanaging