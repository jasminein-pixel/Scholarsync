#include "teacher_login.h"
#include "ui_teacher_login.h"

#include <QLineEdit>

TeacherLogin::TeacherLogin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TeacherLogin)
{
    ui->setupUi(this);

    connect(ui->btnBack, &QPushButton::clicked, this, &TeacherLogin::onBackClicked);
    connect(ui->btnLogin, &QPushButton::clicked, this, &TeacherLogin::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &TeacherLogin::onRegisterClicked);
    connect(ui->btnTogglePassword, &QPushButton::clicked,
            this, &TeacherLogin::onTogglePasswordVisibility);

    // Allow pressing Enter in the password field to submit
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed,
            this, &TeacherLogin::onLoginClicked);

    connect(ui->btnForgotPassword, &QPushButton::clicked,
            this, &TeacherLogin::onForgotPasswordClicked);

    // Fix Login button visibility
ui->btnLogin->setStyleSheet(
    "QPushButton {"
    "  background-color: #1d3fd6;"
    "  color: white;"
    "  border: none;"
    "  border-radius: 8px;"
    "  padding: 12px;"
    "  font-weight: 600;"
    "  font-size: 14px;"
    "}"
    "QPushButton:hover { background-color: #1733b0; }"
);
}

TeacherLogin::~TeacherLogin()
{
    delete ui;
}

void TeacherLogin::clearFields()
{
    ui->emailLineEdit->clear();
    ui->passwordLineEdit->clear();
}

void TeacherLogin::onBackClicked()
{
    emit backRequested();
}

void TeacherLogin::onLoginClicked()
{
    emit loginAttempted(ui->emailLineEdit->text().trimmed(), ui->passwordLineEdit->text());
}

void TeacherLogin::onRegisterClicked()
{
    emit registerRequested();
    // TODO: navigate to a TeacherRegister page once you build one.
}

void TeacherLogin::onTogglePasswordVisibility()
{
    if (ui->passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
        ui->btnTogglePassword->setText(" ");
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
        ui->btnTogglePassword->setText("👁");
    }
}

void TeacherLogin::onForgotPasswordClicked()
{
    emit forgotPasswordRequested();
}