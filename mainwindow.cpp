#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "student_login.h"
#include "teacher_login.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the login pages and add them to the stacked widget.
    // pageRoleSelect (index 0) already lives in mainwindow.ui.
    m_studentLoginPage = new StudentLogin(this);
    m_teacherLoginPage = new TeacherLogin(this);

    ui->stackedWidget->addWidget(m_studentLoginPage); // index 1
    ui->stackedWidget->addWidget(m_teacherLoginPage); // index 2

    ui->stackedWidget->setCurrentIndex(PageRoleSelect);

    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    // Role selection -> navigate to respective login page
    connect(ui->btnStudentRole, &QPushButton::clicked,
            this, &MainWindow::onStudentRoleSelected);
    connect(ui->btnTeacherRole, &QPushButton::clicked,
            this, &MainWindow::onTeacherRoleSelected);

    // Back buttons on each login page return to role selection
    connect(m_studentLoginPage, &StudentLogin::backRequested,
            this, &MainWindow::onBackToRoleSelect);
    connect(m_teacherLoginPage, &TeacherLogin::backRequested,
            this, &MainWindow::onBackToRoleSelect);

    // Login attempts bubble up to MainWindow, which will eventually
    // talk to the Azure SQL backend.
    connect(m_studentLoginPage, &StudentLogin::loginAttempted,
            this, &MainWindow::handleStudentLoginAttempt);
    connect(m_teacherLoginPage, &TeacherLogin::loginAttempted,
            this, &MainWindow::handleTeacherLoginAttempt);
}

void MainWindow::onStudentRoleSelected()
{
    ui->stackedWidget->setCurrentIndex(PageStudentLogin);
}

void MainWindow::onTeacherRoleSelected()
{
    ui->stackedWidget->setCurrentIndex(PageTeacherLogin);
}

void MainWindow::onBackToRoleSelect()
{
    ui->stackedWidget->setCurrentIndex(PageRoleSelect);
}

void MainWindow::handleStudentLoginAttempt(const QString &email, const QString &password)
{
    // TODO: Replace with real authentication against Azure SQL via nanodbc.
    // e.g. query the Students table, hash-compare password, then open the
    // student dashboard window.
    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Failed", "Please enter both email and password.");
        return;
    }

    QMessageBox::information(this, "Student Login",
                             QString("Attempting login for: %1\n(Hook this up to your DB layer.)").arg(email));
}

void MainWindow::handleTeacherLoginAttempt(const QString &email, const QString &password)
{
    // TODO: Replace with real authentication against Azure SQL via nanodbc.
    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Failed", "Please enter both email and password.");
        return;
    }

    QMessageBox::information(this, "Teacher Login",
                             QString("Attempting login for: %1\n(Hook this up to your DB layer.)").arg(email));
}