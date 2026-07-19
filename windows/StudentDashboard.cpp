#include "StudentDashboard.h"
#include "../database/DatabaseManager.h"
#include "../fileupload.h"
#include "../notification.h"
#include "../student_login.h"
#include <QDesktopServices>
#include <QUrl>

// ── Pull in session globals from mainwindow.h ──
extern int currentSID;
extern QString currentUserName;

// ═══════════════════════════════════════════════
//  Style helpers
// ═══════════════════════════════════════════════
QString StudentDashboard::sidebarStyle() {
    return "background-color: #1e3a8a; color: white;";
}
QString StudentDashboard::sidebarBtnStyle() {
    return R"(
        QPushButton {
            color: #bfcfef;
            background-color: transparent;
            border: none;
            text-align: left;
            padding: 12px 20px;
            font-size: 13px;
            border-radius: 0;
        }
        QPushButton:hover {
            background-color: #2d4fa3;
            color: white;
        }
    )";
}
QString StudentDashboard::sidebarBtnActiveStyle() {
    return R"(
        QPushButton {
            color: white;
            background-color: #2d4fa3;
            border-left: 3px solid #60a5fa;
            text-align: left;
            padding: 12px 20px;
            font-size: 13px;
            font-weight: 600;
        }
    )";
}
QString StudentDashboard::tableStyle() {
    return R"(
        QTableWidget {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            gridline-color: #f1f5f9;
            font-size: 13px;
            color: #1e293b;
        }
        QTableWidget::item { padding: 8px; }
        QTableWidget::item:selected {
            background: #eff6ff;
            color: #1e3a8a;
        }
        QHeaderView::section {
            background: #f8fafc;
            color: #64748b;
            font-weight: 600;
            font-size: 12px;
            padding: 10px;
            border: none;
            border-bottom: 1px solid #e2e8f0;
        }
    )";
}
QString StudentDashboard::inputStyle() {
    return R"(
        QLineEdit, QSpinBox, QComboBox {
            border: 1.5px solid #cbd5e1;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            background: white;
            color: #1e293b;
            min-height: 36px;
        }
        QLineEdit:focus, QSpinBox:focus {
            border-color: #3b82f6;
        }
    )";
}
QString StudentDashboard::primaryBtnStyle() {
    return R"(
        QPushButton {
            background-color: #1d4ed8;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 9px 18px;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #1e40af; }
    )";
}
QString StudentDashboard::dangerBtnStyle() {
    return R"(
        QPushButton {
            background-color: #ef4444;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 9px 18px;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #dc2626; }
    )";
}

// ═══════════════════════════════════════════════
//  Sidebar button helpers
// ═══════════════════════════════════════════════
QPushButton* StudentDashboard::sidebarBtn(const QString &text) {
    auto *btn = new QPushButton(text);
    btn->setStyleSheet(sidebarBtnStyle());
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);
    return btn;
}

void StudentDashboard::activateBtn(QPushButton *btn) {
    if (activeBtn) activeBtn->setStyleSheet(sidebarBtnStyle());
    activeBtn = btn;
    btn->setStyleSheet(sidebarBtnActiveStyle());
}

// ═══════════════════════════════════════════════
//  Constructor
// ═══════════════════════════════════════════════
StudentDashboard::StudentDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Student Dashboard");
    resize(1000, 650);
    setStyleSheet("background-color: #f8fafc;");

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ──
    auto *sidebar = new QWidget();
    sidebar->setFixedWidth(210);
    sidebar->setStyleSheet(sidebarStyle());
    auto *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Logo area
    auto *logoArea = new QWidget();
    logoArea->setStyleSheet("background-color: #172554; padding: 4px;");
    auto *logoLayout = new QVBoxLayout(logoArea);
    auto *logoLabel = new QLabel("ScholarSync");
    logoLabel->setStyleSheet(
        "color: white; font-size: 16px; font-weight: 700; padding: 16px 20px 4px;");
    auto *nameLabel = new QLabel(currentUserName.isEmpty() ? "Student" : currentUserName);
    nameLabel->setStyleSheet(
        "color: #93c5fd; font-size: 12px; padding: 0 20px 16px;");
    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(nameLabel);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->addWidget(logoArea);

    // Nav label
    auto *navLabel = new QLabel("NAVIGATION");
    navLabel->setStyleSheet(
        "color: #64748b; font-size: 10px; font-weight: 700; "
        "padding: 16px 20px 8px; letter-spacing: 1px;");
    sideLayout->addWidget(navLabel);

    // Nav buttons
    btnSkills    = sidebarBtn("My Skills");
    btnInterests = sidebarBtn("My Interests");
    btnBrowse    = sidebarBtn("Browse Projects");
    btnCV        = sidebarBtn("Upload CV");
    btnRecommend = sidebarBtn("Recommendations");
    btnInbox     = sidebarBtn("Inbox");

    for (auto *b : {btnSkills, btnInterests, btnBrowse, btnCV, btnRecommend, btnInbox})
        sideLayout->addWidget(b);

    sideLayout->addStretch();

    // Logout button
    auto *btnLogout = new QPushButton("Logout");
    btnLogout->setStyleSheet(dangerBtnStyle());
    btnLogout->setCursor(Qt::PointingHandCursor);
    btnLogout->setMinimumHeight(44);

    sideLayout->addWidget(btnLogout);

    connect(btnLogout, &QPushButton::clicked, this, [this]()
    {
        // Clear student session
        currentSID = -1;
        currentUserName.clear();

        StudentLogin *login = new StudentLogin();
        login->clearFields();
        login->show();

        close();
    });



    // ── Content stack ──
    stack = new QStackedWidget();
    stack->addWidget(buildSkillsScreen());       // 0
    stack->addWidget(buildInterestsScreen());    // 1
    stack->addWidget(buildBrowseScreen());       // 2
    stack->addWidget(buildCVScreen());           // 3
    stack->addWidget(buildRecommendScreen());    // 4
    stack->addWidget(buildInboxScreen());        // 5

    root->addWidget(sidebar);
    root->addWidget(stack);

    // ── Wire sidebar buttons ──
    connect(btnSkills, &QPushButton::clicked, this, [this]{
        activateBtn(btnSkills);
        stack->setCurrentIndex(0);
        loadSkills();
    });
    connect(btnInterests, &QPushButton::clicked, this, [this]{
        activateBtn(btnInterests);
        stack->setCurrentIndex(1);
        loadInterests();
    });
    connect(btnBrowse, &QPushButton::clicked, this, [this]{
        activateBtn(btnBrowse);
        stack->setCurrentIndex(2);
        loadProjects();
    });
    connect(btnCV, &QPushButton::clicked, this, [this]{
        activateBtn(btnCV);
        stack->setCurrentIndex(3);
    });
    connect(btnRecommend, &QPushButton::clicked, this, [this]{
        activateBtn(btnRecommend);
        stack->setCurrentIndex(4);
        loadRecommendations();
    });
    connect(btnInbox, &QPushButton::clicked, this, [this]{
        activateBtn(btnInbox);
        stack->setCurrentIndex(5);
        loadInbox();
    });

    // Default screen
    activateBtn(btnSkills);
    loadSkills();
}

StudentDashboard::~StudentDashboard() {}

// ═══════════════════════════════════════════════
//  Screen builders
// ═══════════════════════════════════════════════

// ── Helper: screen title ──
static QLabel* screenTitle(const QString &text) {
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 20px; font-weight: 700; color: #0f172a; "
        "background: transparent; margin-bottom: 4px;");
    return lbl;
}

static QLabel* screenSub(const QString &text) {
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 13px; color: #64748b; background: transparent; margin-bottom: 16px;");
    return lbl;
}

// ══════════════
//  SKILLS SCREEN
// ══════════════
QWidget* StudentDashboard::buildSkillsScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("My Skills"));
    layout->addWidget(screenSub("Manage your technical skills and proficiency levels."));

    // Table
    skillsTable = new QTableWidget(0, 3);
    skillsTable->setHorizontalHeaderLabels({"Skill", "Proficiency (1-10)", "Action"});
    skillsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    skillsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    skillsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    skillsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    skillsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    skillsTable->verticalHeader()->hide();
    skillsTable->setStyleSheet(tableStyle());
    skillsTable->setMinimumHeight(300);
    layout->addWidget(skillsTable);

    // Add skill row
    auto *addRow = new QHBoxLayout();
    skillNameInput = new QLineEdit();
    skillNameInput->setPlaceholderText("Skill name (e.g. Python)");
    skillNameInput->setStyleSheet(inputStyle());
    skillProfInput = new QSpinBox();
    skillProfInput->setRange(1, 10);
    skillProfInput->setValue(5);
    skillProfInput->setStyleSheet(inputStyle());
    skillProfInput->setFixedWidth(80);

    auto *addBtn = new QPushButton("Add Skill");
    addBtn->setStyleSheet(primaryBtnStyle());
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &StudentDashboard::addSkill);

    addRow->addWidget(skillNameInput);
    addRow->addWidget(new QLabel("Proficiency:"));
    addRow->addWidget(skillProfInput);
    addRow->addWidget(addBtn);
    layout->addLayout(addRow);
    layout->addStretch();

    return w;
}

// ══════════════════
//  INTERESTS SCREEN
// ══════════════════
QWidget* StudentDashboard::buildInterestsScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("My Interests"));
    layout->addWidget(screenSub("Add research areas and topics you're passionate about."));

    interestsList = new QListWidget();
    interestsList->setStyleSheet(R"(
        QListWidget {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            font-size: 13px;
            color: #1e293b;
            padding: 4px;
        }
        QListWidget::item {
            padding: 10px 12px;
            border-radius: 4px;
        }
        QListWidget::item:selected {
            background: #eff6ff;
            color: #1e3a8a;
        }
        QListWidget::item:hover { background: #f8fafc; }
    )");
    interestsList->setMinimumHeight(300);
    layout->addWidget(interestsList);

    auto *addRow = new QHBoxLayout();
    interestInput = new QLineEdit();
    interestInput->setPlaceholderText("e.g. Machine Learning, Robotics");
    interestInput->setStyleSheet(inputStyle());

    auto *addBtn = new QPushButton("Add Interest");
    addBtn->setStyleSheet(primaryBtnStyle());
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &StudentDashboard::addInterest);

    auto *delBtn = new QPushButton("Remove Selected");
    delBtn->setStyleSheet(dangerBtnStyle());
    delBtn->setCursor(Qt::PointingHandCursor);
    connect(delBtn, &QPushButton::clicked, this, &StudentDashboard::deleteInterest);

    addRow->addWidget(interestInput);
    addRow->addWidget(addBtn);
    addRow->addWidget(delBtn);
    layout->addLayout(addRow);
    layout->addStretch();

    return w;
}

// ═════════════════════
//  BROWSE PROJECTS SCREEN
// ═════════════════════
QWidget* StudentDashboard::buildBrowseScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Browse Projects"));
    layout->addWidget(screenSub("Explore active research projects available for application."));

    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("🔄  Refresh");
    refreshBtn->setStyleSheet(primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &StudentDashboard::loadProjects);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    projectsTable = new QTableWidget(0, 6);
    projectsTable->setHorizontalHeaderLabels({
        "Project Name", "Department", "Duration", "Spots", "Preference", "Action"});
    projectsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    projectsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    projectsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    projectsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    projectsTable->verticalHeader()->hide();
    projectsTable->setStyleSheet(tableStyle());
    layout->addWidget(projectsTable);

    return w;
}

// ═══════════════
//  CV SCREEN
// ═══════════════
QWidget* StudentDashboard::buildCVScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(16);

    layout->addWidget(screenTitle("Upload CV"));
    layout->addWidget(screenSub("Upload your CV/Resume so professors can review your profile."));

    // Card
    auto *card = new QWidget();
    card->setStyleSheet(R"(
        QWidget {
            background: white;
            border: 2px dashed #cbd5e1;
            border-radius: 12px;
        }
    )");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(16);
    cardLayout->setAlignment(Qt::AlignCenter);

    auto *icon = new QLabel("📄");
    icon->setStyleSheet("font-size: 48px; border: none; background: transparent;");
    icon->setAlignment(Qt::AlignCenter);

    auto *cardTitle = new QLabel("Select your CV file");
    cardTitle->setStyleSheet(
        "font-size: 16px; font-weight: 600; color: #0f172a; "
        "border: none; background: transparent;");
    cardTitle->setAlignment(Qt::AlignCenter);

    auto *cardSub = new QLabel("Supported formats: PDF, DOCX");
    cardSub->setStyleSheet(
        "font-size: 13px; color: #64748b; border: none; background: transparent;");
    cardSub->setAlignment(Qt::AlignCenter);

    auto *uploadBtn = new QPushButton("Choose File & Upload");
    uploadBtn->setStyleSheet(primaryBtnStyle());
    uploadBtn->setMinimumHeight(44);
    uploadBtn->setCursor(Qt::PointingHandCursor);
    connect(uploadBtn, &QPushButton::clicked, this, &StudentDashboard::uploadCV);

    cvStatusLabel = new QLabel("");
    cvStatusLabel->setAlignment(Qt::AlignCenter);
    cvStatusLabel->setStyleSheet(
        "font-size: 13px; color: #16a34a; border: none; background: transparent;");
    cvStatusLabel->setWordWrap(true);

    cardLayout->addWidget(icon);
    cardLayout->addWidget(cardTitle);
    cardLayout->addWidget(cardSub);
    cardLayout->addWidget(uploadBtn);
    cardLayout->addWidget(cvStatusLabel);

    layout->addWidget(card);
    layout->addStretch();

    return w;
}

// ══════════════════════
//  RECOMMENDATIONS SCREEN
// ══════════════════════
QWidget* StudentDashboard::buildRecommendScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Recommendations"));
    layout->addWidget(screenSub(
        "Projects recommended based on your skills and interests, ranked by match score."));

    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("Refresh");
    refreshBtn->setStyleSheet(primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked,
            this, &StudentDashboard::loadRecommendations);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    recommendTable = new QTableWidget(0, 4);
    recommendTable->setHorizontalHeaderLabels({
        "Project Name", "Department", "Match Score", "Action"});
    recommendTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    recommendTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    recommendTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    recommendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recommendTable->verticalHeader()->hide();
    recommendTable->setStyleSheet(tableStyle());
    layout->addWidget(recommendTable);

    return w;
}

// ═══════════════
//  INBOX SCREEN
// ═══════════════
QWidget* StudentDashboard::buildInboxScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Inbox"));
    layout->addWidget(screenSub("Notifications about your applications and credits."));

    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("🔄  Refresh");
    refreshBtn->setStyleSheet(primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &StudentDashboard::loadInbox);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    inboxList = new QListWidget();
    inboxList->setStyleSheet(R"(
        QListWidget {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            font-size: 13px;
            color: #1e293b;
            padding: 4px;
        }
        QListWidget::item {
            padding: 14px 16px;
            border-bottom: 1px solid #f1f5f9;
            border-radius: 4px;
        }
        QListWidget::item:selected {
            background: #eff6ff;
            color: #1e3a8a;
        }
    )");
    layout->addWidget(inboxList);

    return w;
}

// ═══════════════════════════════════════════════
//  Data loading functions
// ═══════════════════════════════════════════════

void StudentDashboard::loadSkills()
{
    skillsTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.prepareAndExecute(
        "SELECT SkillName, Proficiency FROM SkillList WHERE SID = ?",
        {currentSID});

    int row = 0;
    while (q.next()) {
        skillsTable->insertRow(row);
        skillsTable->setItem(row, 0,
            new QTableWidgetItem(q.value(0).toString()));
        skillsTable->setItem(row, 1,
            new QTableWidgetItem(q.value(1).toString()));

        // Delete button
        auto *delBtn = new QPushButton("Remove");
        delBtn->setStyleSheet(dangerBtnStyle());
        delBtn->setCursor(Qt::PointingHandCursor);
        QString skillName = q.value(0).toString();
        connect(delBtn, &QPushButton::clicked, this, [this, skillName]{
            auto &db = DatabaseManager::instance();
            db.prepareAndExecute(
                "DELETE FROM SkillList WHERE SID = ? AND SkillName = ?",
                {currentSID, skillName});
            loadSkills();
        });
        skillsTable->setCellWidget(row, 2, delBtn);
        row++;
    }

    if (row == 0) {
        skillsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No skills added yet. Add one below!");
        empty->setForeground(QColor("#94a3b8"));
        skillsTable->setItem(0, 0, empty);
    }
}

void StudentDashboard::addSkill()
{
    QString name = skillNameInput->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Missing", "Please enter a skill name.");
        return;
    }
    int prof = skillProfInput->value();

    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    // Check duplicate
    auto check = db.prepareAndExecute(
        "SELECT COUNT(*) FROM SkillList WHERE SID = ? AND SkillName = ?",
        {currentSID, name});
    check.next();
    if (check.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Duplicate", "You already have this skill.");
        return;
    }

    db.prepareAndExecute(
        "INSERT INTO SkillList (SID, SkillName, Proficiency) VALUES (?, ?, ?)",
        {currentSID, name, prof});

    skillNameInput->clear();
    skillProfInput->setValue(5);
    loadSkills();
}

void StudentDashboard::deleteSkill()
{
    int row = skillsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Select", "Please select a skill to remove.");
        return;
    }
    QString skillName = skillsTable->item(row, 0)->text();
    auto &db = DatabaseManager::instance();
    db.prepareAndExecute(
        "DELETE FROM SkillList WHERE SID = ? AND SkillName = ?",
        {currentSID, skillName});
    loadSkills();
}

void StudentDashboard::loadInterests()
{
    interestsList->clear();
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.prepareAndExecute(
        "SELECT Preference FROM StudentDetails WHERE SID = ?",
        {currentSID});

    if (q.next()) {
        QString prefs = q.value(0).toString();
        if (!prefs.isEmpty()) {
            QStringList items = prefs.split(",", Qt::SkipEmptyParts);
            for (auto &item : items)
                interestsList->addItem(item.trimmed());
        }
    }
}

void StudentDashboard::addInterest()
{
    QString interest = interestInput->text().trimmed();
    if (interest.isEmpty()) return;

    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    // Get current preferences
    auto q = db.prepareAndExecute(
        "SELECT Preference FROM StudentDetails WHERE SID = ?", {currentSID});
    QString current = "";
    if (q.next()) current = q.value(0).toString();

    QString updated = current.isEmpty() ? interest : current + ", " + interest;

    db.prepareAndExecute(
        "UPDATE StudentDetails SET Preference = ? WHERE SID = ?",
        {updated, currentSID});

    interestInput->clear();
    loadInterests();
}

void StudentDashboard::deleteInterest()
{
    QListWidgetItem *item = interestsList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Select", "Please select an interest to remove.");
        return;
    }
    QString toRemove = item->text();

    auto &db = DatabaseManager::instance();
    auto q = db.prepareAndExecute(
        "SELECT Preference FROM StudentDetails WHERE SID = ?", {currentSID});
    if (q.next()) {
        QString current = q.value(0).toString();
        QStringList items = current.split(",", Qt::SkipEmptyParts);
        items.removeIf([&](const QString &s){
            return s.trimmed().toLower() == toRemove.toLower();
        });
        QString updated = items.join(", ");
        db.prepareAndExecute(
            "UPDATE StudentDetails SET Preference = ? WHERE SID = ?",
            {updated, currentSID});
    }
    loadInterests();
}

void StudentDashboard::loadProjects()
{
    projectsTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.executeQuery(
        "SELECT PID, ProjectName, department, expectedDuration, vacantSpot, preference "
        "FROM ProjectDetails WHERE status = 'Active' AND vacantSpot > 0");

    int row = 0;
    while (q.next()) {
        projectsTable->insertRow(row);
        int pid = q.value(0).toInt();
        projectsTable->setItem(row, 0, new QTableWidgetItem(q.value(1).toString()));
        projectsTable->setItem(row, 1, new QTableWidgetItem(q.value(2).toString()));
        projectsTable->setItem(row, 2, new QTableWidgetItem(q.value(3).toString()));
        projectsTable->setItem(row, 3, new QTableWidgetItem(q.value(4).toString()));
        projectsTable->setItem(row, 4, new QTableWidgetItem(q.value(5).toString()));

        // Apply button
        auto *applyBtn = new QPushButton("Apply");
        applyBtn->setStyleSheet(primaryBtnStyle());
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this, pid]{
            auto &db = DatabaseManager::instance();

            // Check if already applied
            auto check = db.prepareAndExecute(
                "SELECT COUNT(*) FROM Applications WHERE SID = ? AND PID = ?",
                {currentSID, pid});
            check.next();
            if (check.value(0).toInt() > 0) {
                QMessageBox::information(this, "Already Applied",
                    "You have already applied to this project.");
                return;
            }

            db.prepareAndExecute(
                "INSERT INTO Applications (SID, PID, Status, EngineScore, message) "
                "VALUES (?, ?, 'reviewing', 0, 'Application submitted via ScholarSync')",
                {currentSID, pid});

            QMessageBox::information(this, "Applied!",
                "Your application has been submitted successfully.");
            loadProjects();
        });
        projectsTable->setCellWidget(row, 5, applyBtn);
        row++;
    }

    if (row == 0) {
        projectsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No active projects available right now.");
        empty->setForeground(QColor("#94a3b8"));
        projectsTable->setItem(0, 0, empty);
    }
}

void StudentDashboard::uploadCV()
{
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    QSqlDatabase database = db.database();  // store in local variable first

    File f;
    bool result = f.uploadCV(
        QString::number(currentSID),
        currentUserName,
        database    // pass the local variable, not a temporary
    );

    if (result) {
        cvStatusLabel->setStyleSheet(
            "font-size: 13px; color: #16a34a; border: none; background: transparent;");
        cvStatusLabel->setText("CV uploaded successfully!");
    } else {
        cvStatusLabel->setStyleSheet(
            "font-size: 13px; color: #dc2626; border: none; background: transparent;");
        cvStatusLabel->setText("Upload cancelled or failed.");
    }
}

void StudentDashboard::loadRecommendations()
{
    recommendTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    // Get student skills
    auto skillQ = db.prepareAndExecute(
        "SELECT SkillName FROM SkillList WHERE SID = ?", {currentSID});
    QStringList studentSkills;
    while (skillQ.next())
        studentSkills << skillQ.value(0).toString().toLower();

    // Get all active projects
    auto projQ = db.executeQuery(
        "SELECT PID, ProjectName, department FROM ProjectDetails "
        "WHERE status = 'Active' AND vacantSpot > 0");

    QVector<QPair<int, QString>> scored; // {score, row data}
    QVector<QStringList> rows;

    while (projQ.next()) {
        int pid = projQ.value(0).toInt();
        QString name = projQ.value(1).toString();
        QString dept = projQ.value(2).toString();

        // Get required skills for this project
        auto reqQ = db.prepareAndExecute(
            "SELECT skillName FROM skillRequirement WHERE PID = ?", {pid});
        int matches = 0, total = 0;
        while (reqQ.next()) {
            total++;
            QString req = reqQ.value(0).toString().toLower();
            if (studentSkills.contains(req)) matches++;
        }

        int score = total > 0 ? (matches * 100 / total) : 0;
        scored.append({score, name});
        rows.append({name, dept, QString::number(score) + "%", QString::number(pid)});
    }

    // Sort by score descending
    QVector<int> indices(rows.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](int a, int b){
        return scored[a].first > scored[b].first;
    });

    for (int i : indices) {
        int row = recommendTable->rowCount();
        recommendTable->insertRow(row);
        recommendTable->setItem(row, 0, new QTableWidgetItem(rows[i][0]));
        recommendTable->setItem(row, 1, new QTableWidgetItem(rows[i][1]));

        // Score badge
        auto *scoreItem = new QTableWidgetItem(rows[i][2]);
        scoreItem->setTextAlignment(Qt::AlignCenter);
        int score = scored[i].first;
        if (score >= 70)
            scoreItem->setForeground(QColor("#16a34a"));
        else if (score >= 40)
            scoreItem->setForeground(QColor("#d97706"));
        else
            scoreItem->setForeground(QColor("#dc2626"));
        recommendTable->setItem(row, 2, scoreItem);

        // Apply button
        int pid = rows[i][3].toInt();
        auto *applyBtn = new QPushButton("Apply");
        applyBtn->setStyleSheet(primaryBtnStyle());
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this, pid]{
            auto &db = DatabaseManager::instance();
            auto check = db.prepareAndExecute(
                "SELECT COUNT(*) FROM Applications WHERE SID = ? AND PID = ?",
                {currentSID, pid});
            check.next();
            if (check.value(0).toInt() > 0) {
                QMessageBox::information(this, "Already Applied",
                    "You have already applied to this project.");
                return;
            }
            db.prepareAndExecute(
                "INSERT INTO Applications (SID, PID, Status, EngineScore, message) "
                "VALUES (?, ?, 'reviewing', 0, 'Application submitted via ScholarSync')",
                {currentSID, pid});
            QMessageBox::information(this, "Applied!",
                "Your application has been submitted.");
        });
        recommendTable->setCellWidget(row, 3, applyBtn);
    }

    if (recommendTable->rowCount() == 0) {
        recommendTable->insertRow(0);
        auto *empty = new QTableWidgetItem(
            "No recommendations yet. Add skills to get matched!");
        empty->setForeground(QColor("#94a3b8"));
        recommendTable->setItem(0, 0, empty);
    }
}

void StudentDashboard::loadInbox()
{
    inboxList->clear();
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.prepareAndExecute(
        "SELECT Info, Type, ReceivedAt FROM StudentInbox "
        "WHERE SID = ? ORDER BY ReceivedAt DESC",
        {currentSID});

    while (q.next()) {
        QString info = q.value(0).toString();
        QString type = q.value(1).toString();
        QString time = q.value(2).toString();

        QString icon = type == "CREDIT ALERT" ? "⭐ " : "📬 ";
        QString display = icon + info + "\n" + time;

        auto *item = new QListWidgetItem(display);
        if (type == "CREDIT ALERT")
            item->setForeground(QColor("#d97706"));
        inboxList->addItem(item);
    }

    if (inboxList->count() == 0) {
        inboxList->addItem("Your inbox is empty.");
    }
}

QWidget* StudentDashboard::makePlaceholder(const QString &text) {
    auto *w = new QWidget();
    auto *l = new QVBoxLayout(w);
    auto *lbl = new QLabel(text);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("font-size: 16px; color: #94a3b8;");
    l->addWidget(lbl);
    return w;
}