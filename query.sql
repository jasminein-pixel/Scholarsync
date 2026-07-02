DROP TABLE IF EXISTS Applications;
DROP TABLE IF EXISTS CompletedProjects;
DROP TABLE IF EXISTS skillRequirement;
DROP TABLE IF EXISTS ProjectDetails;
DROP TABLE IF EXISTS SkillList;
DROP TABLE IF EXISTS TeacherDetails;
DROP TABLE IF EXISTS StudentDetails;


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
    ContactInfo VARCHAR(20),

    -- NEW FEATURES
    CvPath VARCHAR(500),
    CreditScore INT DEFAULT 0
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
        FOREIGN KEY (SID) REFERENCES StudentDetails(SID)
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
);


CREATE TABLE skillRequirement
(
    PID INT,
    skillName VARCHAR(255),
    requiredProficiency INT,
    CONSTRAINT requirementPK PRIMARY KEY (PID, skillName),
    FOREIGN KEY (PID) REFERENCES ProjectDetails(PID)
);


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
);


CREATE TABLE CompletedProjects
(
    SID INT,
    PID INT,
    CompletionDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    PerformanceRating INT, -- 1-10

    FOREIGN KEY (SID) REFERENCES StudentDetails(SID),
    FOREIGN KEY (PID) REFERENCES ProjectDetails(PID)
);




INSERT INTO StudentDetails
(Name, Email, PasswordHash, Department, Program, Level, Semester, Preference, ContactInfo, CvPath, CreditScore)
VALUES
('Rahul Shrestha', 'rahulshrestha102@gmail.com', 'gs823eg3872uh82',
 'Dept Of Comp Sci & Eng.', 'Computer Engineering', 'Undergrad', 2,
 'Remote', '9769818436', 'uploads/cv/rahul.pdf', 0),

('Jasmine Shrestha', 'xthajasmine123@gmail.com', 'dbrjfn3u4h9384',
 'Dept Of Comp Sci & Eng.', 'Computer Engineering', 'Undergrad', 2,
 'Remote', '9676767676', 'uploads/cv/jasmine.pdf', 0);


INSERT INTO TeacherDetails
(Name, Email, PasswordHash, Department, Qualification, ContactInfo)
VALUES
('Saubhagya Acharya', 'saubhagya123@gmail.com', 'bdiu4h84uhbhe48247',
 'Department Of Electrical Engineering', 'Masters', '9876543210');


INSERT INTO SkillList VALUES (1 , 'Django' , 9);
INSERT INTO SkillList VALUES (1 , 'C++' , 10);
INSERT INTO SkillList VALUES (1 , 'Rust' , 8);
INSERT INTO SkillList VALUES (1 , 'Embedded System Designing' , 4);
INSERT INTO SkillList VALUES (2 , 'Django' , 10);
INSERT INTO SkillList VALUES (2 , 'JavaScript' , 9);


INSERT INTO ProjectDetails
(ProjectName, TID, ProjectDesc, vacantSpot, expectedDuration, department, preference, applicants, status)
VALUES
('Remodelling of Faradays Electric Architecture & Induction', 1,
 'This project aims to study and simulate Faraday’s electromagnetic induction principles.',
 5, '5 Months', 'Department Of Electrical Engineering', 'On-Site', 0, 'Active');


INSERT INTO skillRequirement VALUES (1 , 'Rust' , 10);
INSERT INTO skillRequirement VALUES (1 , 'C++' , 10);
INSERT INTO skillRequirement VALUES (1 , 'Java' , 5);


INSERT INTO Applications (SID, PID, Status, EngineScore, message)
VALUES (1, 1, 'reviewing', 100, 'sir malaii ni linuu naa');

INSERT INTO Applications (SID, PID, Status, EngineScore, message)
VALUES (2, 1, 'reviewing', 100, 'helklkoo sirrrr');


INSERT INTO CompletedProjects (SID, PID, PerformanceRating)
VALUES (1, 1, 9);

INSERT INTO CompletedProjects (SID, PID, PerformanceRating)
VALUES (2, 1, 8);


CREATE TRIGGER UpdateCreditScore
ON CompletedProjects
AFTER INSERT
AS
BEGIN
    UPDATE sd
    SET CreditScore = (
        SELECT ISNULL(SUM(cp.PerformanceRating * 10), 0)
        FROM CompletedProjects cp
        WHERE cp.SID = sd.SID
    )
    FROM StudentDetails sd
    INNER JOIN inserted i ON sd.SID = i.SID;
END;


UPDATE StudentDetails
SET CreditScore = (
    SELECT ISNULL(SUM(PerformanceRating * 10), 0)
    FROM CompletedProjects cp
    WHERE cp.SID = StudentDetails.SID
);