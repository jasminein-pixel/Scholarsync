#include "student_login.h"
#include "ui_student_login.h"

#include <QLineEdit>

StudentLogin::StudentLogin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentLogin)
{
    ui->setupUi(this);

    connect(ui->btnBack, &QPushButton::clicked, this, &StudentLogin::onBackClicked);
    connect(ui->btnLogin, &QPushButton::clicked, this, &StudentLogin::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &StudentLogin::onRegisterClicked);
    connect(ui->btnTogglePassword, &QPushButton::clicked,
            this, &StudentLogin::onTogglePasswordVisibility);

    // Allow pressing Enter in the password field to submit
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed,
            this, &StudentLogin::onLoginClicked);
    connect(ui->btnForgotPassword, &QPushButton::clicked,
            this, &StudentLogin::onForgotPasswordClicked);

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


StudentLogin::~StudentLogin()
{
    delete ui;
}

void StudentLogin::clearFields()
{
    ui->emailLineEdit->clear();
    ui->passwordLineEdit->clear();
}

void StudentLogin::onBackClicked()
{
    emit backRequested();
}

void StudentLogin::onLoginClicked()
{
    emit loginAttempted(ui->emailLineEdit->text().trimmed(), ui->passwordLineEdit->text());
}

void StudentLogin::onRegisterClicked()
{
    emit registerRequested();
    // TODO: navigate to a StudentRegister page once you build one.
}

void StudentLogin::onTogglePasswordVisibility()
{
    if (ui->passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
        ui->btnTogglePassword->setText(" ");
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
        ui->btnTogglePassword->setText("👁");
    }
}

void StudentLogin::onForgotPasswordClicked()
{
    emit forgotPasswordRequested();
}