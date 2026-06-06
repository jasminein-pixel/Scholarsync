#include <nanodbc/nanodbc.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;
//First install OpenSSL: pacman -S mingw-w64-x86_64-openssl
//g++ -std=c++17 AuthManager.cpp nanodbc/nanodbc/nanodbc.cpp -I./nanodbc -lodbc32 -lssl -lcrypto -o auth.exe
//./auth.exe

// ─────────────────────────────────────────
//  CONNECTION
// ─────────────────────────────────────────
nanodbc::connection getConnection() {
    return nanodbc::connection(
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=;"
        "Encrypt=yes;"
        "Connection Timeout=30");
}

// ─────────────────────────────────────────
//  HASHING UTILITIES
// ─────────────────────────────────────────

// SHA-256 core function
string sha256(string input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input.c_str(), input.size(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];

    return ss.str();
}

// Generate a random 16-byte salt → 32 char hex string
string generateSalt() {
    unsigned char salt[16];
    RAND_bytes(salt, 16);

    stringstream ss;
    for (int i = 0; i < 16; i++)
        ss << hex << setw(2) << setfill('0') << (int)salt[i];

    return ss.str();
}

// Hash password with salt → returns "salt:hash"
string hashPassword(string password) {
    string salt = generateSalt();
    string hash = sha256(salt + password);
    return salt + ":" + hash;  // stored as one string in DB
}

// Verify password against stored "salt:hash"
bool verifyPassword(string inputPassword, string storedValue) {
    string salt      = storedValue.substr(0, 32);   // first 32 chars = salt
    string storedHash = storedValue.substr(33);      // after ":" = hash
    string inputHash  = sha256(salt + inputPassword);
    return inputHash == storedHash;
}

// ─────────────────────────────────────────
//  STUDENT AUTH
// ─────────────────────────────────────────

bool registerStudent(
    string name, string email, string password,
    string department, string program, string level,
    int semester, string preference, string contact)
{
    try {
        auto conn = getConnection();

        // Check if email already exists
        string checkQuery = "SELECT COUNT(*) FROM StudentDetails WHERE Email = ?";
        auto stmt = nanodbc::statement(conn, checkQuery);
        stmt.bind(0, email.c_str());
        auto check = nanodbc::execute(stmt);
        check.next();
        if (check.get<int>(0) > 0) {
            cout << "Email already registered!\n";
            return false;
        }

        // Hash the password
        string hashed = hashPassword(password);

        // Insert new student
        string query =
            "INSERT INTO StudentDetails "
            "(Name, Email, PasswordHash, Department, Program, Level, Semester, Preference, ContactInfo) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

        auto insertStmt = nanodbc::statement(conn, query);
        insertStmt.bind(0, name.c_str());
        insertStmt.bind(1, email.c_str());
        insertStmt.bind(2, hashed.c_str());
        insertStmt.bind(3, department.c_str());
        insertStmt.bind(4, program.c_str());
        insertStmt.bind(5, level.c_str());
        insertStmt.bind(6, &semester);
        insertStmt.bind(7, preference.c_str());
        insertStmt.bind(8, contact.c_str());
        nanodbc::execute(insertStmt);

        cout << "Student registered successfully!\n";
        return true;
    }
    catch (const nanodbc::database_error& e) {
        cout << "Registration failed: " << e.what() << "\n";
        return false;
    }
}

bool loginStudent(string email, string password) {
    try {
        auto conn = getConnection();

        // Fetch stored hash for this email
        string query =
            "SELECT SID, Name, PasswordHash FROM StudentDetails WHERE Email = ?";
        auto stmt = nanodbc::statement(conn, query);
        stmt.bind(0, email.c_str());
        auto result = nanodbc::execute(stmt);

        if (!result.next()) {
            cout << "No student found with that email.\n";
            return false;
        }

        int    sid        = result.get<int>(0);
        string name       = result.get<string>(1);
        string storedHash = result.get<string>(2);

        // Verify password
        if (verifyPassword(password, storedHash)) {
            cout << "Login successful!\n";
            cout << "Welcome, " << name << " (SID: " << sid << ")\n";
            return true;
        } else {
            cout << "Incorrect password.\n";
            return false;
        }
    }
    catch (const nanodbc::database_error& e) {
        cout << "Login failed: " << e.what() << "\n";
        return false;
    }
}

// ─────────────────────────────────────────
//  TEACHER AUTH
// ─────────────────────────────────────────

bool registerTeacher(
    string name, string email, string password,
    string department, string qualification, string contact)
{
    try {
        auto conn = getConnection();

        // Check if email already exists
        string checkQuery = "SELECT COUNT(*) FROM TeacherDetails WHERE Email = ?";
        auto stmt = nanodbc::statement(conn, checkQuery);
        stmt.bind(0, email.c_str());
        auto check = nanodbc::execute(stmt);
        check.next();
        if (check.get<int>(0) > 0) {
            cout << "Email already registered!\n";
            return false;
        }

        // Hash the password
        string hashed = hashPassword(password);

        // Insert new teacher
        string query =
            "INSERT INTO TeacherDetails "
            "(Name, Email, PasswordHash, Department, Qualification, ContactInfo) "
            "VALUES (?, ?, ?, ?, ?, ?)";

        auto insertStmt = nanodbc::statement(conn, query);
        insertStmt.bind(0, name.c_str());
        insertStmt.bind(1, email.c_str());
        insertStmt.bind(2, hashed.c_str());
        insertStmt.bind(3, department.c_str());
        insertStmt.bind(4, qualification.c_str());
        insertStmt.bind(5, contact.c_str());
        nanodbc::execute(insertStmt);

        cout << "Teacher registered successfully!\n";
        return true;
    }
    catch (const nanodbc::database_error& e) {
        cout << "Registration failed: " << e.what() << "\n";
        return false;
    }
}

bool loginTeacher(string email, string password) {
    try {
        auto conn = getConnection();

        string query =
            "SELECT TID, Name, PasswordHash FROM TeacherDetails WHERE Email = ?";
        auto stmt = nanodbc::statement(conn, query);
        stmt.bind(0, email.c_str());
        auto result = nanodbc::execute(stmt);

        if (!result.next()) {
            cout << "No teacher found with that email.\n";
            return false;
        }

        int    tid        = result.get<int>(0);
        string name       = result.get<string>(1);
        string storedHash = result.get<string>(2);

        if (verifyPassword(password, storedHash)) {
            cout << "Login successful!\n";
            cout << "Welcome, Prof. " << name << " (TID: " << tid << ")\n";
            return true;
        } else {
            cout << "Incorrect password.\n";
            return false;
        }
    }
    catch (const nanodbc::database_error& e) {
        cout << "Login failed: " << e.what() << "\n";
        return false;
    }
}

// ─────────────────────────────────────────
//  MAIN — test everything
// ─────────────────────────────────────────
int main() {
    cout << "===== ScholarSync Auth System =====\n\n";

    // Test student registration
    cout << "-- Registering a new student --\n";
    registerStudent(
        "Test Student",
        "teststudent@gmail.com",
        "mypassword123",
        "Dept Of Comp Sci & Eng.",
        "Computer Engineering",
        "Undergrad",
        2,
        "Remote",
        "9800000000"
    );

    // Test student login - correct password
    cout << "\n-- Student Login (correct password) --\n";
    loginStudent("teststudent@gmail.com", "mypassword123");

    // Test student login - wrong password
    cout << "\n-- Student Login (wrong password) --\n";
    loginStudent("teststudent@gmail.com", "wrongpassword");

    // Test teacher registration
    cout << "\n-- Registering a new teacher --\n";
    registerTeacher(
        "Test Professor",
        "testprof@gmail.com",
        "profpassword456",
        "Dept Of Comp Sci & Eng.",
        "PhD",
        "9811111111"
    );

    // Test teacher login
    cout << "\n-- Teacher Login (correct password) --\n";
    loginTeacher("testprof@gmail.com", "profpassword456");

    return 0;
}