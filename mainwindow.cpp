#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "views/student_login.h"
#include "views/teacher_login.h"
#include "views/forgotpassword.h"
#include "windows/RegisterWindow.h"
#include "auth/AuthManager.h"
#include "database/DatabaseManager.h"
#include "windows/student/StudentDashboard.h"
#include "windows/teacher/TeacherDashboard.h"
#include <QMessageBox>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap logo(":/scholarsync_logo.png");
    ui->logoLabel->setPixmap(
        logo.scaled(300, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation)
    );
    ui->logoLabel->setAlignment(Qt::AlignCenter);
    ui->logoLabel->setText("");
    ui->appNameLabel->hide();
    ui->taglineLabel->hide();


    m_studentLoginPage = new StudentLogin(this);
    m_teacherLoginPage = new TeacherLogin(this);
    m_forgotPasswordPage = new ForgotPasswordPage(this);

    ui->stackedWidget->addWidget(m_studentLoginPage);
    ui->stackedWidget->addWidget(m_teacherLoginPage);
    ui->stackedWidget->addWidget(m_forgotPasswordPage);

    ui->stackedWidget->setCurrentIndex(PageRoleSelect);

    DatabaseManager::instance().connect();

    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(ui->btnStudentRole, &QPushButton::clicked,
            this, &MainWindow::onStudentRoleSelected);
    connect(ui->btnTeacherRole, &QPushButton::clicked,
            this, &MainWindow::onTeacherRoleSelected);

    connect(m_studentLoginPage, &StudentLogin::backRequested,
            this, &MainWindow::onBackToRoleSelect);
    connect(m_teacherLoginPage, &TeacherLogin::backRequested,
            this, &MainWindow::onBackToRoleSelect);

    connect(m_studentLoginPage, &StudentLogin::loginAttempted,
            this, &MainWindow::handleStudentLoginAttempt);
    connect(m_teacherLoginPage, &TeacherLogin::loginAttempted,
            this, &MainWindow::handleTeacherLoginAttempt);

    connect(m_studentLoginPage, &StudentLogin::registerRequested,
            this, &MainWindow::onStudentRegisterRequested);
    connect(m_teacherLoginPage, &TeacherLogin::registerRequested,
            this, &MainWindow::onTeacherRegisterRequested);

    connect(m_studentLoginPage, &StudentLogin::forgotPasswordRequested,
            this, &MainWindow::onForgotPasswordRequested);
    connect(m_teacherLoginPage, &TeacherLogin::forgotPasswordRequested,
            this, &MainWindow::onForgotPasswordRequested);
    connect(m_forgotPasswordPage, &ForgotPasswordPage::backRequested,
            this, &MainWindow::onBackToRoleSelect);
    connect(m_forgotPasswordPage, &ForgotPasswordPage::passwordResetSuccess,
            this, &MainWindow::onPasswordResetSuccess);
}

void MainWindow::goToStudentLogin()
{
    ui->stackedWidget->setCurrentIndex(PageStudentLogin);
}

void MainWindow::goToTeacherLogin()
{
    ui->stackedWidget->setCurrentIndex(PageTeacherLogin);
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
    if (email.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Login Failed",
                             "Please enter both email and password.");
        return;
    }

    if (AuthManager::loginStudent(email, password))
    {
        auto q = DatabaseManager::instance().prepareAndExecute(
            "SELECT SID, Name FROM StudentDetails WHERE Email = ?",
            {email});
        if (q.next())
        {
            currentSID = q.value(0).toInt();
            currentUserName = q.value(1).toString();
        }

        StudentDashboard *dashboard = new StudentDashboard();
        dashboard->setAttribute(Qt::WA_DeleteOnClose);
        dashboard->show();
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "Login Failed",
                             "Incorrect email or password.");
    }
}

void MainWindow::handleTeacherLoginAttempt(const QString &email, const QString &password)
{
    if (email.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Login Failed",
                             "Please enter both email and password.");
        return;
    }

    if (AuthManager::loginTeacher(email, password))
    {
        auto q = DatabaseManager::instance().prepareAndExecute(
            "SELECT TID, Name FROM TeacherDetails WHERE Email = ?",
            {email});
        if (q.next())
        {
            currentTID = q.value(0).toInt();
            currentUserName = q.value(1).toString();
        }

        TeacherDashboard *dashboard = new TeacherDashboard();
        dashboard->setAttribute(Qt::WA_DeleteOnClose);
        dashboard->show();
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "Login Failed",
                             "Incorrect email or password.");
    }
}

void MainWindow::onStudentRegisterRequested()
{
    RegisterWindow *reg = new RegisterWindow();
    reg->setAttribute(Qt::WA_DeleteOnClose);
    reg->show();
}

void MainWindow::onTeacherRegisterRequested()
{
    RegisterWindow *reg = new RegisterWindow();
    reg->setAttribute(Qt::WA_DeleteOnClose);
    reg->show();
}

void MainWindow::onForgotPasswordRequested()
{
    m_forgotPasswordPage->clearFields();
    ui->stackedWidget->setCurrentIndex(PageForgotPassword);
}

void MainWindow::onPasswordResetSuccess()
{
    ui->stackedWidget->setCurrentIndex(PageRoleSelect);
}