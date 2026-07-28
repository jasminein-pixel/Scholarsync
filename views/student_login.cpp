#include "student_login.h"
#include "ui_student_login.h"

#include <QLineEdit>

// ─────────────────────────────────────────────────────────────────
//  Shared stylesheet for the Show/Hide toggle button.
//  Matches the app's blue theme (#1d3fd6 family).
// ─────────────────────────────────────────────────────────────────
static QString toggleBtnStyle()
{
    return
        "QPushButton {"
        "  background-color: transparent;"
        "  border: 1.5px solid #cbd5e1;"
        "  border-radius: 6px;"
        "  color: #1d3fd6;"
        "  font-size: 11px;"
        "  font-weight: 600;"
        "  padding: 2px 8px;"
        "  min-width: 44px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #eff6ff;"
        "  border-color: #1d3fd6;"
        "}";
}

StudentLogin::StudentLogin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentLogin)
{
    ui->setupUi(this);

    // Style the Show/Hide toggle button to match the app theme
    ui->btnTogglePassword->setStyleSheet(toggleBtnStyle());

    connect(ui->btnBack,           &QPushButton::clicked, this, &StudentLogin::onBackClicked);
    connect(ui->btnLogin,          &QPushButton::clicked, this, &StudentLogin::onLoginClicked);
    connect(ui->btnRegister,       &QPushButton::clicked, this, &StudentLogin::onRegisterClicked);
    connect(ui->btnTogglePassword, &QPushButton::clicked, this, &StudentLogin::onTogglePasswordVisibility);

    // Allow pressing Enter in the password field to submit
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &StudentLogin::onLoginClicked);
    connect(ui->btnForgotPassword, &QPushButton::clicked,   this, &StudentLogin::onForgotPasswordClicked);

    // Primary login button style
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
}

// ── Toggle password visibility, switching text between "Show" and "Hide" ──
void StudentLogin::onTogglePasswordVisibility()
{
    if (ui->passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
        ui->btnTogglePassword->setText("Hide");
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
        ui->btnTogglePassword->setText("Show");
    }
}

void StudentLogin::onForgotPasswordClicked()
{
    emit forgotPasswordRequested();
}
