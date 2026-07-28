#include "forgotpassword.h"
#include "../auth/AuthManager.h"
#include "../database/DatabaseManager.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

ForgotPasswordPage::ForgotPasswordPage(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Forgot Password");
    setFixedSize(420, 720);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    stepStack = new QStackedWidget(this);
    mainLayout->addWidget(stepStack);

    buildStep0();
    buildStep1();
    buildStep2();

    stepStack->addWidget(step0Widget);
    stepStack->addWidget(step1Widget);
    stepStack->addWidget(step2Widget);
    stepStack->setCurrentIndex(0);
}

ForgotPasswordPage::~ForgotPasswordPage() {}

QString ForgotPasswordPage::inputStyle()
{
    return
        "QLineEdit {"
        "  border: 1.5px solid #c0c8e0;"
        "  border-radius: 8px;"
        "  padding: 10px 12px;"
        "  font-size: 13px;"
        "  background: #ffffff;"
        "  color: #16213e;"
        "}"
        "QLineEdit:focus { border: 1.5px solid #2f54eb; }";
}

QString ForgotPasswordPage::primaryBtnStyle()
{
    return
        "QPushButton {"
        "  background-color: #1d3fd6;"
        "  color: #ffffff;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 12px;"
        "  font-weight: 600;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #1733b0; }";
}

QString ForgotPasswordPage::backBtnStyle()
{
    return
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  color: #1d3fd6;"
        "  font-weight: 600;"
        "  font-size: 13px;"
        "}";
}

QString ForgotPasswordPage::statusStyle()
{
    return
        "color: #dc2626;"
        "font-size: 12px;"
        "background: transparent;";
}

QString ForgotPasswordPage::labelStyle()
{
    return
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #16213e;"
        "background: transparent;";
}

QString ForgotPasswordPage::questionLabelStyle()
{
    return
        "font-size: 13px;"
        "font-weight: 600;"
        "color: #16213e;"
        "background: transparent;"
        "margin-top: 12px;";
}

// ── Step 0: Enter Email and Role ──
void ForgotPasswordPage::buildStep0()
{
    step0Widget = new QWidget();
    auto *layout = new QVBoxLayout(step0Widget);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(8);

    backBtn = new QPushButton("\u2190 Back to Login");
    backBtn->setObjectName("backButton");
    backBtn->setMaximumWidth(140);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(backBtnStyle());
    connect(backBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onBackClicked);
    layout->addWidget(backBtn);

    auto *spacer = new QLabel("");
    spacer->setStyleSheet("background: transparent;");
    layout->addWidget(spacer);

    auto *title = new QLabel("Reset Password");
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #16213e; background: transparent;");
    layout->addWidget(title);

    auto *subtitle = new QLabel("Enter your email to begin the reset process.");
    subtitle->setStyleSheet("font-size: 12px; color: #6b7280; background: transparent; margin-bottom: 20px;");
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    // Role selector
    auto *roleLbl = new QLabel("I am a:");
    roleLbl->setStyleSheet(labelStyle());
    layout->addWidget(roleLbl);
    roleBox = new QComboBox();
    roleBox->addItems({"Student", "Teacher"});
    roleBox->setMinimumHeight(42);
    roleBox->setStyleSheet(
        "QComboBox {"
        "  border: 1.5px solid #c0c8e0;"
        "  border-radius: 8px;"
        "  padding: 8px 12px;"
        "  font-size: 13px;"
        "  background: #ffffff;"
        "  color: #16213e;"
        "}"
        "QComboBox:focus { border: 1.5px solid #2f54eb; }"
        "QComboBox::drop-down { border: none; }");
    layout->addWidget(roleBox);

    layout->addSpacing(8);

    // Email
    auto *emailLbl = new QLabel("Email Address");
    emailLbl->setStyleSheet(labelStyle());
    layout->addWidget(emailLbl);
    emailInput = new QLineEdit();
    emailInput->setPlaceholderText("Enter your registered email");
    emailInput->setMinimumHeight(42);
    emailInput->setStyleSheet(inputStyle());
    layout->addWidget(emailInput);

    statusLabel0 = new QLabel("");
    statusLabel0->setStyleSheet(statusStyle());
    statusLabel0->setWordWrap(true);
    layout->addWidget(statusLabel0);

    layout->addSpacing(12);

    nextBtn = new QPushButton("Next");
    nextBtn->setObjectName("primaryButton");
    nextBtn->setMinimumHeight(44);
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setStyleSheet(primaryBtnStyle());
    connect(nextBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onNextClicked);
    layout->addWidget(nextBtn);

    layout->addStretch();
}

// ── Step 1: Security Questions ──
void ForgotPasswordPage::buildStep1()
{
    step1Widget = new QWidget();
    auto *layout = new QVBoxLayout(step1Widget);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(8);

    auto *title = new QLabel("Security Questions");
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #16213e; background: transparent;");
    layout->addWidget(title);

    auto *subtitle = new QLabel("Answer the questions you set during registration.");
    subtitle->setStyleSheet("font-size: 12px; color: #6b7280; background: transparent; margin-bottom: 12px;");
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    question1Label = new QLabel("Which city were you born in?");
    question1Label->setStyleSheet(questionLabelStyle());
    layout->addWidget(question1Label);
    answer1Input = new QLineEdit();
    answer1Input->setPlaceholderText("Your answer");
    answer1Input->setMinimumHeight(42);
    answer1Input->setStyleSheet(inputStyle());
    layout->addWidget(answer1Input);

    question2Label = new QLabel("What is your favourite colour?");
    question2Label->setStyleSheet(questionLabelStyle());
    layout->addWidget(question2Label);
    answer2Input = new QLineEdit();
    answer2Input->setPlaceholderText("Your answer");
    answer2Input->setMinimumHeight(42);
    answer2Input->setStyleSheet(inputStyle());
    layout->addWidget(answer2Input);

    question3Label = new QLabel("What is your favourite food?");
    question3Label->setStyleSheet(questionLabelStyle());
    layout->addWidget(question3Label);
    answer3Input = new QLineEdit();
    answer3Input->setPlaceholderText("Your answer");
    answer3Input->setMinimumHeight(42);
    answer3Input->setStyleSheet(inputStyle());
    layout->addWidget(answer3Input);

    statusLabel1 = new QLabel("");
    statusLabel1->setStyleSheet(statusStyle());
    statusLabel1->setWordWrap(true);
    layout->addWidget(statusLabel1);

    layout->addSpacing(12);

    verifyBtn = new QPushButton("Verify Answers");
    verifyBtn->setObjectName("primaryButton");
    verifyBtn->setMinimumHeight(44);
    verifyBtn->setCursor(Qt::PointingHandCursor);
    verifyBtn->setStyleSheet(primaryBtnStyle());
    connect(verifyBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onVerifyClicked);
    layout->addWidget(verifyBtn);

    layout->addStretch();
}

// ── Step 2: New Password ──
void ForgotPasswordPage::buildStep2()
{
    step2Widget = new QWidget();
    auto *layout = new QVBoxLayout(step2Widget);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(8);

    auto *title = new QLabel("New Password");
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #16213e; background: transparent;");
    layout->addWidget(title);

    auto *subtitle = new QLabel("Create a strong new password for your account.");
    subtitle->setStyleSheet("font-size: 12px; color: #6b7280; background: transparent; margin-bottom: 16px;");
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    auto *newLbl = new QLabel("New Password");
    newLbl->setStyleSheet(labelStyle());
    layout->addWidget(newLbl);
    newPasswordInput = new QLineEdit();
    newPasswordInput->setPlaceholderText("At least 6 characters");
    newPasswordInput->setEchoMode(QLineEdit::Password);
    newPasswordInput->setMinimumHeight(42);
    newPasswordInput->setStyleSheet(inputStyle());
    layout->addWidget(newPasswordInput);

    layout->addSpacing(8);

    auto *confirmLbl = new QLabel("Confirm Password");
    confirmLbl->setStyleSheet(labelStyle());
    layout->addWidget(confirmLbl);
    confirmPasswordInput = new QLineEdit();
    confirmPasswordInput->setPlaceholderText("Re-enter your password");
    confirmPasswordInput->setEchoMode(QLineEdit::Password);
    confirmPasswordInput->setMinimumHeight(42);
    confirmPasswordInput->setStyleSheet(inputStyle());
    layout->addWidget(confirmPasswordInput);

    statusLabel2 = new QLabel("");
    statusLabel2->setStyleSheet(statusStyle());
    statusLabel2->setWordWrap(true);
    layout->addWidget(statusLabel2);

    layout->addSpacing(12);

    resetBtn = new QPushButton("Reset Password");
    resetBtn->setObjectName("primaryButton");
    resetBtn->setMinimumHeight(44);
    resetBtn->setCursor(Qt::PointingHandCursor);
    resetBtn->setStyleSheet(primaryBtnStyle());
    connect(resetBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onResetClicked);
    layout->addWidget(resetBtn);

    layout->addStretch();
}

// ── Slot implementations ──

void ForgotPasswordPage::clearFields()
{
    emailInput->clear();
    answer1Input->clear();
    answer2Input->clear();
    answer3Input->clear();
    newPasswordInput->clear();
    confirmPasswordInput->clear();
    statusLabel0->clear();
    statusLabel1->clear();
    statusLabel2->clear();
    roleBox->setCurrentIndex(0);
    stepStack->setCurrentIndex(0);
}

void ForgotPasswordPage::onBackClicked()
{
    emit backRequested();
}

void ForgotPasswordPage::onNextClicked()
{
    QString email = emailInput->text().trimmed();
    if (email.isEmpty()) {
        statusLabel0->setText("Please enter your email address.");
        return;
    }
    bool isStudent = (roleBox->currentText() == "Student");
    if (!AuthManager::emailExists(email, isStudent)) {
        statusLabel0->setText("No account found with that email.");
        return;
    }
    m_email = email;
    m_role  = roleBox->currentText();
    statusLabel0->clear();
    stepStack->setCurrentIndex(1);
}

void ForgotPasswordPage::onVerifyClicked()
{
    QString ans1 = answer1Input->text().trimmed();
    QString ans2 = answer2Input->text().trimmed();
    QString ans3 = answer3Input->text().trimmed();

    if (ans1.isEmpty() || ans2.isEmpty() || ans3.isEmpty()) {
        statusLabel1->setText("Please answer all three questions.");
        return;
    }
    bool isStudent = (m_role == "Student");
    if (!AuthManager::verifySecurityAnswers(m_email, ans1, ans2, ans3, isStudent)) {
        statusLabel1->setText("One or more answers are incorrect. Please try again.");
        return;
    }
    statusLabel1->clear();
    stepStack->setCurrentIndex(2);
}

void ForgotPasswordPage::onResetClicked()
{
    QString newPass = newPasswordInput->text();
    QString confirm = confirmPasswordInput->text();

    if (newPass.length() < 6) {
        statusLabel2->setText("Password must be at least 6 characters.");
        return;
    }
    if (newPass != confirm) {
        statusLabel2->setText("Passwords do not match.");
        return;
    }
    bool isStudent = (m_role == "Student");
    if (AuthManager::updatePassword(m_email, newPass, isStudent)) {
        statusLabel2->setStyleSheet("color: #16a34a; font-size: 12px; background: transparent;");
        statusLabel2->setText("Password reset successful! Returning to login...");
        emit passwordResetSuccess();
    } else {
        statusLabel2->setText("Something went wrong. Please try again.");
    }
}
