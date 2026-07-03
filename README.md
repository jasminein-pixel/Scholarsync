## ScholarSync

ScholarSync is a desktop application built using **C++** and **Qt** that connects to an **Azure SQL Database**. The project currently implements a secure authentication system for students with database connectivity, registration, and login functionality.

---

## Features

- Azure SQL Database connectivity
- Student registration
- Student login
- Secure password hashing using SHA-256
- Random salt generation for passwords
- Prepared SQL statements to prevent SQL injection
- Basic UI for Login, Registration, Student Dashboard, and Teacher Dashboard

---

## Technologies Used

- C++
- Qt 6
- Qt Widgets
- Qt SQL Module
- Microsoft Azure SQL Database
- ODBC Driver 18 for SQL Server

---

## Project Structure

```
ScholarSync/
│
├── auth/
│   ├── AuthManager.h
│   └── AuthManager.cpp
│
├── database/
│   ├── DatabaseManager.h
│   └── DatabaseManager.cpp
│
├── windows/
│   ├── LoginWindow
│   ├── RegisterWindow
│   ├── StudentDashboard
│   └── TeacherDashboard
│
└── main.cpp
```

---

## Authentication

### Student Registration

The registration process:

- Checks if the email already exists.
- Generates a random salt.
- Hashes the password using SHA-256.
- Stores the student's information, password hash, and salt.
- Initializes the student's credit to **0**.

### Student Login

The login process:

- Searches for the student using the provided email.
- Retrieves the stored password hash and salt.
- Hashes the entered password using the stored salt.
- Verifies the password before allowing access.

---

## Database

The application connects to an Azure SQL Database using Qt's SQL module through an ODBC connection.

Connection string format:

```cpp
DRIVER={ODBC Driver 18 for SQL Server};
SERVER=<server>;
DATABASE=<database>;
UID=<username>;
PWD=<password>;
Encrypt=yes;
TrustServerCertificate=yes;
```

> Replace the placeholder credentials with your own database credentials before running the application.

---

## Security

Current security features include:

- SHA-256 password hashing
- Random salt generation
- Passwords are never stored in plain text
- Prepared SQL queries with parameter binding

---

## Current Status

- Database connection implemented
- Student registration implemented
- Student login implemented
- Authentication manager implemented
- Basic UI files created for:
  - Login Window
  - Register Window
  - Student Dashboard
  - Teacher Dashboard

---

## Requirements

- Qt 6.x
- C++17 or later
- Microsoft ODBC Driver 18 for SQL Server
- Azure SQL Database

---
