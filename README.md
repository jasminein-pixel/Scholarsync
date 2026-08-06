# ScholarSync
### Research Collaboration Platform for Student–Faculty Matching

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6-green)
![Azure SQL](https://img.shields.io/badge/Azure-SQL_Server-blue)
![Azure Blob Storage](https://img.shields.io/badge/Azure-Blob_Storage-0089D6)
![License](https://img.shields.io/badge/License-MIT-yellow)
# ScholarSync Documentation

## Overview

ScholarSync is a desktop-based research collaboration platform designed to simplify the process of connecting students with faculty research opportunities. The system allows students to create profiles, upload CVs, browse available projects, and apply to research positions. Faculty members can publish projects, review applications, and select suitable candidates.

The platform also includes a recommendation engine that ranks projects based on a student's profile, helping students discover relevant research opportunities more efficiently.

---

# Objectives

- Simplify student–faculty research collaboration
- Replace manual application workflows
- Provide intelligent project recommendations
- Securely manage user accounts and documents
- Maintain a centralized database of research opportunities

---

# Technology Stack

| Component | Technology |
|------------|------------|
| Programming Language | C++ |
| GUI Framework | Qt 6 (Qt Widgets) |
| Database | Azure SQL Database |
| Database Driver | QODBC (ODBC Driver 18 for SQL Server) |
| Cloud Storage | Azure Blob Storage |
| Authentication | SHA-256 Password Hashing with Random Salt |
| IDE | Qt Creator |
| Version Control | Git & GitHub |

---

# System Architecture

ScholarSync follows a modular desktop application architecture.

```
+--------------------+
|      Qt GUI        |
+---------+----------+
          |
          v
+--------------------+
| Business Logic     |
| Recommendation     |
| Authentication     |
| Project Management |
+---------+----------+
          |
          v
+--------------------+
| Azure SQL Database |
+---------+----------+
          |
          v
+--------------------+
| Azure Blob Storage |
+--------------------+
```

---

# Core Modules

## Authentication

Responsible for

- User Registration
- Login
- Logout
- Password Reset
- Session Validation

Features

- Student and Faculty accounts
- SHA-256 password hashing
- Random salt generated for every user
- Security questions for password recovery

---

## Student Module

Students can

- Complete profile
- Upload CV (PDF)
- Browse projects
- Receive recommendations
- Apply to projects
- Track application status

---

## Faculty Module

Faculty members can

- Create research projects
- Edit projects
- Delete projects
- Review applications
- Accept or reject applicants

---

## Recommendation Engine

ScholarSync generates project recommendations using a weighted scoring algorithm.

### Evaluation Criteria

| Factor | Weight |
|----------|---------|
| Technical Skills | 70% |
| Research Interests | 20% |
| Preferences | 10% |

Additional matching techniques include

- Keyword similarity
- Fuzzy text matching
- Faculty area-of-expertise comparison

The final weighted score is normalized into a compatibility score ranging from **1–10**.

---

# Database

Azure SQL Database stores

- User accounts
- Student profiles
- Faculty profiles
- Research projects
- Applications
- Interests
- Recommendation data

Relationships are maintained using foreign keys to preserve data integrity.

---

# File Storage

Student CVs are stored in Azure Blob Storage instead of the SQL database.

Benefits include

- Reduced database size
- Faster retrieval
- Scalable document storage
- Secure cloud access

Only PDF files are accepted for upload.

---

# Security

## Password Security

Passwords are never stored in plain text.

Each password is

- Salted with a unique random value
- Hashed using SHA-256
- Stored as the resulting hash

---

## Authentication

Separate authentication workflows exist for

- Students
- Faculty

Access to features depends on the authenticated user's role.

---

## Input Validation

ScholarSync validates

- Login credentials
- Registration fields
- Project information
- Uploaded CV format
- Duplicate entries

---

# Project Workflow

## Student

```
Register
      ↓
Login
      ↓
Complete Profile
      ↓
Upload CV
      ↓
Browse Projects
      ↓
Receive Recommendations
      ↓
Apply
      ↓
Track Status
```

---

## Faculty

```
Register
      ↓
Login
      ↓
Create Project
      ↓
Receive Applications
      ↓
Review Applicants
      ↓
Accept / Reject
```

---

# Recommendation Workflow

```
Student Profile
        ↓
Extract Skills
        ↓
Extract Interests
        ↓
Compare with Projects
        ↓
Keyword Matching
        ↓
Fuzzy Text Matching
        ↓
Weighted Score Calculation
        ↓
Compatibility Score (1–10)
        ↓
Rank Projects
```

---

# Key Features

## Student

- Secure login
- CV upload
- Recommendation engine
- Project browsing
- Application tracking

## Faculty

- Project management
- Application review
- Candidate selection

## System

- Azure SQL integration
- Azure Blob Storage
- Secure authentication
- Intelligent recommendations

---

# Performance

Typical operations

| Operation | Approximate Time |
|------------|------------------|
| User Login | 2–3 seconds |
| Database Connection | ~5 seconds |
| Recommendation Generation | Less than 15 second (typical dataset) |

Performance may vary depending on network connectivity.

---

# Error Handling

The application handles

- Database connection failures
- Invalid login credentials
- Duplicate registration
- Missing required fields
- Unsupported file uploads
- Blob Storage upload errors

User-friendly dialogs are displayed whenever an operation fails.

---

# Limitations

- Desktop-only application
- Internet connection required
- Recommendation quality depends on profile completeness
- Faculty availability is manually managed

---

# Future Improvements

- Email notifications
- Real-time messaging
- AI-enhanced recommendation model
- Multi-file document support
- Student messaging system
- Interview scheduling
- Research progress tracking
- Mobile application
- Analytics dashboard

---

# Repository Structure


```text
ScholarSync/
├── auth/
│   ├── AuthManager.cpp
│   └── AuthManager.h
│
├── database/
│   ├── DatabaseManager.cpp
│   └── DatabaseManager.h
│
├── models/
│   ├── Student.cpp
│   ├── Student.h
│   ├── Teacher.cpp
│   └── Teacher.h
│
├── services/
│   ├── fileupload.cpp
│   ├── fileupload.h
│   ├── MatchingEngine.cpp
│   ├── MatchingEngine.h
│   ├── modification.cpp
│   ├── modification.h
│   ├── notification.cpp
│   └── notification.h
│
├── unused/
│   ├── application.cpp
│   ├── application.h
│   ├── applicationManager.cpp
│   ├── applicationManager.h
│   └── connect.cpp
│
├── views/
│   ├── forgotpassword.cpp
│   ├── forgotpassword.h
│   ├── student_login.cpp
│   ├── student_login.h
│   ├── student_login.ui
│   ├── teacher_login.cpp
│   ├── teacher_login.h
│   └── teacher_login.ui
│
├── windows/
│   ├── student/
│   │   ├── PieChartWidget.cpp
│   │   ├── PieChartWidget.h
│   │   ├── StudentBrowseScreen.cpp
│   │   ├── StudentCVScreen.cpp
│   │   ├── StudentDashboard.cpp
│   │   ├── StudentDashboard.h
│   │   ├── StudentInboxScreen.cpp
│   │   ├── StudentInterestsScreen.cpp
│   │   ├── StudentProfileScreen.cpp
│   │   ├── StudentRecommendScreen.cpp
│   │   ├── StudentSkillsScreen.cpp
│   │   └── StudentStyles.h
│   │
│   ├── teacher/
│   │   ├── TeacherApplicantsScreen.cpp
│   │   ├── TeacherDashboard.cpp
│   │   ├── TeacherDashboard.h
│   │   ├── TeacherPostScreen.cpp
│   │   ├── TeacherProjectsScreen.cpp
│   │   └── TeacherStyles.h
│   │
│   ├── RegisterWindow.cpp
│   └── RegisterWindow.h
│
├── .gitignore
├── CMakeLists.txt
├── main.cpp
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
└── README.md
```

# License

This project is licensed under the MIT License.

See the LICENSE file for complete details.

