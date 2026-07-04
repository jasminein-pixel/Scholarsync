#include "RegisterWindow.h"
#include "../auth/AuthManager.h"
#include "../database/DatabaseManager.h"
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Register");
    setFixedSize(460, 700);
    setStyleSheet("background-color: #f0f4ff;");
    setupUI();
}

RegisterWindow::~RegisterWindow() {}

void RegisterWindow::setupUI()
{
    // Outer layout holds scroll area
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // Scroll area so all fields are reachable
    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background-color: #f0f4ff;");

    auto *container = new QWidget();
    container->setStyleSheet("background-color: #f0f4ff;");

    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(10);

    // ── Back button ──
    auto *backBtn = new QPushButton("← Back");
    backBtn->setObjectName("backButton");
    backBtn->setMaximumWidth(80);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &RegisterWindow::onBackClicked);
    layout->addWidget(backBtn);

    // ── Title ──
    auto *title = new QLabel("Create Account");
    title->setStyleSheet(
        "font-size: 22px; font-weight: 700; color: #16213e; "
        "background: transparent; margin-top: 8px;");
    layout->addWidget(title);

    auto *subtitle = new QLabel("Join ScholarSync today.");
    subtitle->setStyleSheet(
        "font-size: 12px; color: #6b7280; background: transparent; margin-bottom: 8px;");
    layout->addWidget(subtitle);

    // ── Helper lambda to add labelled fields ──
    auto addField = [&](const QString &labelText,
                        QLineEdit *&field,
                        const QString &placeholder,
                        QVBoxLayout *targetLayout)
    {
        auto *lbl = new QLabel(labelText);
        lbl->setStyleSheet(
            "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
        field = new QLineEdit();
        field->setPlaceholderText(placeholder);
        field->setMinimumHeight(42);
        field->setStyleSheet(
            "QLineEdit {"
            "  border: 1.5px solid #c0c8e0;"
            "  border-radius: 8px;"
            "  padding: 8px 12px;"
            "  font-size: 13px;"
            "  background: #ffffff;"
            "  color: #16213e;"
            "}"
            "QLineEdit:focus { border: 1.5px solid #2f54eb; }");
        targetLayout->addWidget(lbl);
        targetLayout->addWidget(field);
    };

    auto addCombo = [&](const QString &labelText,
                        QComboBox *&combo,
                        const QStringList &items,
                        QVBoxLayout *targetLayout)
    {
        auto *lbl = new QLabel(labelText);
        lbl->setStyleSheet(
            "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
        combo = new QComboBox();
        combo->addItems(items);
        combo->setMinimumHeight(42);
        combo->setStyleSheet(
            "QComboBox {"
            "  border: 1.5px solid #c0c8e0;"
            "  border-radius: 8px;"
            "  padding: 8px 12px;"
            "  font-size: 13px;"
            "  background: #ffffff;"
            "  color: #16213e;"
            "}"
            "QComboBox:focus { border: 1.5px solid #2f54eb; }"
            "QComboBox::drop-down { border: none; width: 24px; }"
            "QComboBox QAbstractItemView {"
            "  background: #ffffff;"
            "  color: #16213e;"
            "  selection-background-color: #eef1ff;"
            "}");
        targetLayout->addWidget(lbl);
        targetLayout->addWidget(combo);
    };

    // ── Role selector ──
    addCombo("I am a:", roleBox, {"Student", "Teacher"}, layout);

    // ── Shared fields ──
    addField("Full Name",         nameInput,            "Enter your full name",   layout);
    addField("Email",             emailInput,           "Enter your email",       layout);
    addField("Password",          passwordInput,        "Create a password",      layout);
    passwordInput->setEchoMode(QLineEdit::Password);
    addField("Confirm Password",  confirmPasswordInput, "Re-enter password",      layout);
    confirmPasswordInput->setEchoMode(QLineEdit::Password);
    addField("Department",        departmentInput,      "Your department",        layout);
    addField("Contact",           contactInput,         "Phone number",           layout);

    // ── Student-only fields ──
    studentFieldsWidget = new QWidget();
    studentFieldsWidget->setStyleSheet("background: transparent;");
    auto *studentLayout = new QVBoxLayout(studentFieldsWidget);
    studentLayout->setContentsMargins(0, 0, 0, 0);
    studentLayout->setSpacing(10);

    addField("Program", programInput, "e.g. Computer Engineering", studentLayout);
    addCombo("Level", levelBox, {"Undergrad", "Postgrad", "PhD"}, studentLayout);

    auto *semLabel = new QLabel("Semester");
    semLabel->setStyleSheet(
        "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
    semesterBox = new QSpinBox();
    semesterBox->setRange(1, 8);
    semesterBox->setValue(1);
    semesterBox->setMinimumHeight(42);
    semesterBox->setStyleSheet(
        "QSpinBox {"
        "  border: 1.5px solid #c0c8e0;"
        "  border-radius: 8px;"
        "  padding: 8px 12px;"
        "  font-size: 13px;"
        "  background: #ffffff;"
        "  color: #16213e;"
        "}"
        "QSpinBox:focus { border: 1.5px solid #2f54eb; }");
    studentLayout->addWidget(semLabel);
    studentLayout->addWidget(semesterBox);

    addCombo("Preference", preferenceBox,
             {"Remote", "On-Site", "Hybrid"}, studentLayout);

    layout->addWidget(studentFieldsWidget);

    // ── Teacher-only fields ──
    teacherFieldsWidget = new QWidget();
    teacherFieldsWidget->setStyleSheet("background: transparent;");
    auto *teacherLayout = new QVBoxLayout(teacherFieldsWidget);
    teacherLayout->setContentsMargins(0, 0, 0, 0);
    teacherLayout->setSpacing(10);

    addField("Qualification", qualificationInput,
             "e.g. PhD, Masters", teacherLayout);

    layout->addWidget(teacherFieldsWidget);
    teacherFieldsWidget->hide();

    // ── Status label ──
    statusLabel = new QLabel("");
    statusLabel->setStyleSheet(
        "color: #dc2626; font-size: 12px; background: transparent;");
    statusLabel->setWordWrap(true);
    layout->addWidget(statusLabel);

    // ── Create Account button ──
    auto *registerBtn = new QPushButton("Create Account");
    registerBtn->setMinimumHeight(46);
    registerBtn->setCursor(Qt::PointingHandCursor);
    registerBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #1d3fd6;"
        "  color: #ffffff;"
        "  border: none;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover { background-color: #1733b0; }");
    connect(registerBtn, &QPushButton::clicked,
            this, &RegisterWindow::onRegisterClicked);
    layout->addWidget(registerBtn);
    layout->addStretch();

    // Wire role switcher
    connect(roleBox, &QComboBox::currentTextChanged,
            this, &RegisterWindow::toggleRoleFields);

    scrollArea->setWidget(container);
    outerLayout->addWidget(scrollArea);
}

void RegisterWindow::toggleRoleFields(const QString &role)
{
    studentFieldsWidget->setVisible(role == "Student");
    teacherFieldsWidget->setVisible(role == "Teacher");
}

void RegisterWindow::onBackClicked()
{
    emit backRequested();
    this->close();
}

void RegisterWindow::onRegisterClicked()
{
    if (nameInput->text().isEmpty() || emailInput->text().isEmpty() ||
        passwordInput->text().isEmpty()) {
        statusLabel->setText("Please fill in all required fields.");
        return;
    }

    if (passwordInput->text() != confirmPasswordInput->text()) {
        statusLabel->setText("Passwords do not match.");
        return;
    }

    if (passwordInput->text().length() < 6) {
        statusLabel->setText("Password must be at least 6 characters.");
        return;
    }

    if (!DatabaseManager::instance().isConnected()) {
        DatabaseManager::instance().connect();
    }

    QString role    = roleBox->currentText();
    bool    success = false;

    if (role == "Student") {
        success = AuthManager::registerStudent(
            nameInput->text().trimmed(),
            emailInput->text().trimmed(),
            passwordInput->text(),
            departmentInput->text().trimmed(),
            programInput->text().trimmed(),
            levelBox->currentText(),
            semesterBox->value(),
            preferenceBox->currentText(),
            contactInput->text().trimmed());
    } else {
        success = AuthManager::registerTeacher(
            nameInput->text().trimmed(),
            emailInput->text().trimmed(),
            passwordInput->text(),
            departmentInput->text().trimmed(),
            qualificationInput->text().trimmed(),
            contactInput->text().trimmed());
    }

    if (success) {
        QMessageBox::information(this, "Success",
            "Account created! You can now log in.");
        emit registrationSuccessful();
        this->close();
    } else {
        statusLabel->setText("Registration failed. Email may already be registered.");
    }
}