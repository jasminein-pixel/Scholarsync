-- queries that will be necessary if table have to be reinitialized
-- these queries were used to build tables

CREATE TABLE StudentDetails
(
    SID INT IDENTITY(1,1) PRIMARY KEY,
    Name VARCHAR(255),
    Email VARCHAR(255) UNIQUE NOT NULL,
    PasswordHash VARCHAR(255) NOT NULL,
    Department VARCHAR(255),
    Program VARCHAR(255),
    Level VARCHAR(255),
    Semester INT,
    Preference VARCHAR(50),
    ContactInfo VARCHAR(20)

);
CREATE TABLE TeacherDetails
(
    TID INT IDENTITY(1,1) PRIMARY KEY,
    Name VARCHAR(255),
    Email VARCHAR(255) UNIQUE NOT NULL,
    PasswordHash VARCHAR(255) NOT NULL,
    Department VARCHAR(255),
    Qualification VARCHAR(255),
    ContactInfo VARCHAR(20)
);

CREATE TABLE SkillList
(
    SID INT,
    SkillName VARCHAR(255),
    Proficiency INT,
    CONSTRAINT PK_SkillList PRIMARY KEY (SID, SkillName),
    CONSTRAINT FK_Skill_Student
    FOREIGN KEY (SID)
    REFERENCES StudentDetails(SID)
);

CREATE TABLE ProjectDetails
(
    PID INT IDENTITY(1,1) PRIMARY KEY,
    ProjectName VARCHAR(255),
    TID INT,
    ProjectDesc VARCHAR(2000),
    vacantSpot INT,
    expectedDuration VARCHAR(255),
    department VARCHAR(255),
    preference VARCHAR(255),
    applicants INT,
    status VARCHAR(255),
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (TID) REFERENCES TeacherDetails(TID)
)

CREATE TABLE skillRequirement
(
    PID INT,
    skillName VARCHAR(255),
    requiredProficiency INT,
    FOREIGN KEY (PID) REFERENCES ProjectDetails(PID),
    CONSTRAINT requirementPK PRIMARY KEY (PID, SkillName)

)

CREATE TABLE Applications
(
    AID INT IDENTITY(1,1),
    SID INT,
    PID INT,
    Status VARCHAR(255),
    EngineScore INT,
    message VARCHAR(1000),
    AppliedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (SID, PID),
    FOREIGN KEY (SID) REFERENCES StudentDetails(SID),
    FOREIGN KEY (PID) REFERENCES ProjectDetails(PID)
)


INSERT INTO StudentDetails VALUES('Rahul Shrestha', 'rahulshrestha102@gmail.com', 'gs823eg3872uh82' , 'Dept Of Comp Sci & Eng.', 'Computer Engineering', 'Undergrad', 2 , 'Remote' , '9769818436')
INSERT INTO StudentDetails VALUES('Jasmine Shrestha', 'xthajasmine123@gmail.com', 'dbrjfn3u4h9384' , 'Dept Of Comp Sci & Eng.', 'Computer Engineering', 'Undergrad', 2 , 'Remote' , '9676767676')

INSERT INTO TeacherDetails VALUES('Saubhagya Acharya', 'saubhagya123@gmail.com', 'bdiu4h84uhbhe48247', 'Department Of Electrical Engineering', 'Masters' , '9876543210' )

INSERT INTO SkillList VALUES(1 , 'Django' , 9)
INSERT INTO SkillList VALUES(1 , 'C++' , 10)
INSERT INTO SkillList VALUES(1 , 'Rust' , 8)
INSERT INTO SkillList VALUES(1 , 'Embedded System Designing' , 4)
INSERT INTO SkillList VALUES(2 , 'Django' , 10)
INSERT INTO SkillList VALUES(2 , 'JavaScript' , 9)

INSERT INTO ProjectDetails
(ProjectName, TID, ProjectDesc, vacantSpot, expectedDuration, department, preference, applicants, status)
VALUES ('Remodelling of Faradays Electric Architecture & Induction', 1, 'This project aims to study and simulate Faraday’s electromagnetic induction principles.',5,'5 Months','Department Of Electrical Engineering','On-Site',0,'Active');

INSERT INTO skillRequirement VALUES(1 , 'Rust' , 10)
INSERT INTO skillRequirement VALUES(1 , 'C++' , 10)
INSERT INTO skillRequirement VALUES(1 , 'Java' , 5)


INSERT INTO Applications (SID, PID, Status, EngineScore, message)
VALUES (1, 1, 'reviewing', 100, 'sir malaii ni linuu naa');

INSERT INTO Applications (SID, PID, Status, EngineScore, message)
VALUES (2, 1, 'reviewing', 100, 'helklkoo sirrrr');



DROP TABLE Applications
DROP TABLE skillRequirement
DROP TABLE ProjectDetails
DROP TABLE SkillList
DROP TABLE TeacherDetails
DROP TABLE StudentDetails
