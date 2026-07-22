#include "TeacherDashboard.h"
#include "../database/DatabaseManager.h"
#include "../notification.h"
#include "../teacher_login.h"
#include <QDesktopServices>
#include <QUrl>
#include <numeric>
#include <algorithm>
#include "../mainwindow.h"

extern int currentTID;
extern QString currentUserName;

// ═══════════════════════════════════════════════
//  Style helpers
// ═══════════════════════════════════════════════
QString TeacherDashboard::sidebarStyle()
{
    return "background-color: #1e3a8a; color: white;";
}
QString TeacherDashboard::sidebarBtnStyle()
{
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
QString TeacherDashboard::sidebarBtnActiveStyle()
{
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
QString TeacherDashboard::tableStyle()
{
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
QString TeacherDashboard::inputStyle()
{
    return R"(
        QLineEdit, QSpinBox, QComboBox, QTextEdit {
            border: 1.5px solid #cbd5e1;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            background: white;
            color: #1e293b;
            min-height: 36px;
        }
        QLineEdit:focus, QSpinBox:focus, QTextEdit:focus {
            border-color: #3b82f6;
        }
    )";
}
QString TeacherDashboard::primaryBtnStyle()
{
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
QString TeacherDashboard::successBtnStyle()
{
    return R"(
        QPushButton {
            background-color: #16a34a;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 7px 14px;
            font-size: 12px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #15803d; }
    )";
}
QString TeacherDashboard::dangerBtnStyle()
{
    return R"(
        QPushButton {
            background-color: #ef4444;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 7px 14px;
            font-size: 12px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #dc2626; }
    )";
}

// ═══════════════════════════════════════════════
//  Sidebar helpers
// ═══════════════════════════════════════════════
QPushButton *TeacherDashboard::sidebarBtn(const QString &text)
{
    auto *btn = new QPushButton(text);
    btn->setStyleSheet(sidebarBtnStyle());
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);
    return btn;
}

void TeacherDashboard::activateBtn(QPushButton *btn)
{
    if (activeBtn)
        activeBtn->setStyleSheet(sidebarBtnStyle());
    activeBtn = btn;
    btn->setStyleSheet(sidebarBtnActiveStyle());
}

// ═══════════════════════════════════════════════
//  Constructor
// ═══════════════════════════════════════════════
TeacherDashboard::TeacherDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Teacher Dashboard");
    resize(1100, 680);
    setStyleSheet("background-color: #f8fafc;");

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ──
    auto *sidebar = new QWidget();
    sidebar->setFixedWidth(220);
    sidebar->setStyleSheet(sidebarStyle());
    auto *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Logo area
    auto *logoArea = new QWidget();
    logoArea->setStyleSheet("background-color: #172554; padding: 4px;");
    auto *logoLayout = new QVBoxLayout(logoArea);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    auto *logoLabel = new QLabel("ScholarSync");
    logoLabel->setStyleSheet(
        "color: white; font-size: 16px; font-weight: 700; padding: 16px 20px 4px;");
    auto *nameLabel = new QLabel(
        currentUserName.isEmpty() ? "Professor" : "Prof. " + currentUserName);
    nameLabel->setStyleSheet(
        "color: #93c5fd; font-size: 12px; padding: 0 20px 16px;");
    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(nameLabel);
    sideLayout->addWidget(logoArea);

    // Nav label
    auto *navLabel = new QLabel("NAVIGATION");
    navLabel->setStyleSheet(
        "color: #64748b; font-size: 10px; font-weight: 700; "
        "padding: 16px 20px 8px; letter-spacing: 1px;");
    sideLayout->addWidget(navLabel);

    btnProjects = sidebarBtn("My Projects");
    btnPost = sidebarBtn("Post a Project");
    btnApplicants = sidebarBtn("View Applicants");

    sideLayout->addWidget(btnProjects);
    sideLayout->addWidget(btnPost);
    sideLayout->addWidget(btnApplicants);
    sideLayout->addStretch();

    // Logout button
    auto *btnLogout = new QPushButton("Logout");
    btnLogout->setStyleSheet(dangerBtnStyle());
    btnLogout->setCursor(Qt::PointingHandCursor);
    btnLogout->setMinimumHeight(44);

    sideLayout->addWidget(btnLogout);

    connect(btnLogout, &QPushButton::clicked, this, [this]()
            {
        // Clear teacher session
        currentTID = -1;
        currentUserName.clear();


        MainWindow *mainWin = new MainWindow();
        mainWin->setAttribute(Qt::WA_DeleteOnClose);
        mainWin->TeacherLogin();
        mainWin->show();

        close(); });

    // ── Content stack ──
    stack = new QStackedWidget();
    stack->addWidget(buildProjectsScreen());   // 0
    stack->addWidget(buildPostScreen());       // 1
    stack->addWidget(buildApplicantsScreen()); // 2

    root->addWidget(sidebar);
    root->addWidget(stack);

    // ── Wire buttons ──
    connect(btnProjects, &QPushButton::clicked, this, [this]
            {
        activateBtn(btnProjects);
        stack->setCurrentIndex(0);
        loadMyProjects(); });
    connect(btnPost, &QPushButton::clicked, this, [this]
            {
        activateBtn(btnPost);
        stack->setCurrentIndex(1); });
    connect(btnApplicants, &QPushButton::clicked, this, [this]
            {
        activateBtn(btnApplicants);
        stack->setCurrentIndex(2);
        if (currentPID == -1)
            applicantsTitle->setText(
                "Select a project from 'My Projects' to view its applicants."); });

    // Default
    activateBtn(btnProjects);
    loadMyProjects();
}

TeacherDashboard::~TeacherDashboard() {}

// ═══════════════════════════════════════════════
//  Screen builders
// ═══════════════════════════════════════════════

static QLabel *screenTitle(const QString &text)
{
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 20px; font-weight: 700; color: #0f172a; "
        "background: transparent; margin-bottom: 4px;");
    return lbl;
}
static QLabel *screenSub(const QString &text)
{
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 13px; color: #64748b; background: transparent; margin-bottom: 12px;");
    return lbl;
}

// ══════════════════
//  MY PROJECTS
// ══════════════════
QWidget *TeacherDashboard::buildProjectsScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("My Projects"));
    layout->addWidget(screenSub("All research projects you have posted."));

    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("Refresh");
    refreshBtn->setStyleSheet(primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &TeacherDashboard::loadMyProjects);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    projectsTable = new QTableWidget(0, 6);
    projectsTable->setHorizontalHeaderLabels({"Project Name", "Dept", "Spots Left", "Applicants", "Status", "Action"});
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

// ══════════════════
//  POST PROJECT
// ══════════════════
QWidget *TeacherDashboard::buildPostScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: #f8fafc;");

    auto *container = new QWidget();
    container->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Post a Project"));
    layout->addWidget(screenSub("Fill in the details to post a new research project."));

    // Helper lambda
    auto addField = [&](const QString &label, QLineEdit *&field,
                        const QString &placeholder)
    {
        auto *lbl = new QLabel(label);
        lbl->setStyleSheet(
            "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
        field = new QLineEdit();
        field->setPlaceholderText(placeholder);
        field->setStyleSheet(inputStyle());
        layout->addWidget(lbl);
        layout->addWidget(field);
    };

    addField("Project Name", projNameInput, "e.g. AI-powered Medical Diagnosis");
    addField("Department", projDeptInput, "e.g. Department Of Computer Science");
    addField("Duration", projDurationInput, "e.g. 3 Months");

    // Description
    auto *descLabel = new QLabel("Project Description");
    descLabel->setStyleSheet(
        "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
    projDescInput = new QTextEdit();
    projDescInput->setPlaceholderText("Describe the project goals, methodology...");
    projDescInput->setStyleSheet(inputStyle());
    projDescInput->setMinimumHeight(100);
    layout->addWidget(descLabel);
    layout->addWidget(projDescInput);

    // Spots and Preference row
    auto *row1 = new QHBoxLayout();
    auto *spotsLabel = new QLabel("Vacant Spots");
    spotsLabel->setStyleSheet(
        "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
    projSpotsInput = new QSpinBox();
    projSpotsInput->setRange(1, 20);
    projSpotsInput->setValue(3);
    projSpotsInput->setStyleSheet(inputStyle());

    auto *prefLabel = new QLabel("Preference");
    prefLabel->setStyleSheet(
        "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
    projPrefInput = new QComboBox();
    projPrefInput->addItems({"On-Site", "Remote", "Hybrid"});
    projPrefInput->setStyleSheet(inputStyle());

    auto *col1 = new QVBoxLayout();
    col1->addWidget(spotsLabel);
    col1->addWidget(projSpotsInput);
    auto *col2 = new QVBoxLayout();
    col2->addWidget(prefLabel);
    col2->addWidget(projPrefInput);
    row1->addLayout(col1);
    row1->addLayout(col2);
    layout->addLayout(row1);

    // Skill requirements section
    auto *skillTitle = new QLabel("Required Skills");
    skillTitle->setStyleSheet(
        "font-size: 14px; font-weight: 700; color: #0f172a; "
        "background: transparent; margin-top: 8px;");
    layout->addWidget(skillTitle);

    skillReqList = new QListWidget();
    skillReqList->setStyleSheet(R"(
        QListWidget {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            font-size: 13px;
            color: #1e293b;
            padding: 4px;
            min-height: 100px;
        }
        QListWidget::item { padding: 8px 12px; }
        QListWidget::item:selected { background: #eff6ff; color: #1e3a8a; }
    )");
    layout->addWidget(skillReqList);

    // Add skill row
    auto *skillRow = new QHBoxLayout();
    skillReqNameInput = new QLineEdit();
    skillReqNameInput->setPlaceholderText("Skill name (e.g. Python)");
    skillReqNameInput->setStyleSheet(inputStyle());

    skillReqProfInput = new QSpinBox();
    skillReqProfInput->setRange(1, 10);
    skillReqProfInput->setValue(5);
    skillReqProfInput->setStyleSheet(inputStyle());
    skillReqProfInput->setFixedWidth(80);

    auto *addSkillBtn = new QPushButton("Add");
    addSkillBtn->setStyleSheet(primaryBtnStyle());
    addSkillBtn->setCursor(Qt::PointingHandCursor);
    connect(addSkillBtn, &QPushButton::clicked,
            this, &TeacherDashboard::addSkillRequirement);

    auto *removeSkillBtn = new QPushButton("Remove");
    removeSkillBtn->setStyleSheet(dangerBtnStyle());
    removeSkillBtn->setCursor(Qt::PointingHandCursor);
    connect(removeSkillBtn, &QPushButton::clicked,
            this, &TeacherDashboard::removeSkillRequirement);

    skillRow->addWidget(skillReqNameInput);
    skillRow->addWidget(new QLabel("Min:"));
    skillRow->addWidget(skillReqProfInput);
    skillRow->addWidget(addSkillBtn);
    skillRow->addWidget(removeSkillBtn);
    layout->addLayout(skillRow);

    // Status label
    postStatusLabel = new QLabel("");
    postStatusLabel->setStyleSheet(
        "color: #dc2626; font-size: 12px; background: transparent;");
    postStatusLabel->setWordWrap(true);
    layout->addWidget(postStatusLabel);

    // Post button
    auto *postBtn = new QPushButton("Post Project");
    postBtn->setStyleSheet(primaryBtnStyle());
    postBtn->setMinimumHeight(44);
    postBtn->setCursor(Qt::PointingHandCursor);
    connect(postBtn, &QPushButton::clicked, this, &TeacherDashboard::postProject);
    layout->addWidget(postBtn);
    layout->addStretch();

    scroll->setWidget(container);

    auto *outerLayout = new QVBoxLayout(w);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);

    return w;
}

// ══════════════════
//  APPLICANTS
// ══════════════════
QWidget *TeacherDashboard::buildApplicantsScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    applicantsTitle = new QLabel("Select a project to view applicants.");
    applicantsTitle->setStyleSheet(
        "font-size: 20px; font-weight: 700; color: #0f172a; background: transparent;");
    layout->addWidget(applicantsTitle);
    layout->addWidget(screenSub(
        "Review applicants, view their skills and CV, then approve or reject."));

    applicantsTable = new QTableWidget(0, 7);
    applicantsTable->setHorizontalHeaderLabels({"Student Name", "Program", "Semester", "Match Score",
                                                "Message", "CV", "Action"});
    applicantsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    applicantsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    applicantsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    applicantsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    applicantsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    applicantsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    applicantsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    applicantsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    applicantsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    applicantsTable->verticalHeader()->hide();
    applicantsTable->setStyleSheet(tableStyle());
    layout->addWidget(applicantsTable);

    return w;
}

// ═══════════════════════════════════════════════
//  Data functions
// ═══════════════════════════════════════════════

void TeacherDashboard::loadMyProjects()
{
    projectsTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    auto q = db.prepareAndExecute(
        "SELECT PID, ProjectName, department, vacantSpot, applicants, status "
        "FROM ProjectDetails WHERE TID = ?",
        {currentTID});

    int row = 0;
    while (q.next())
    {
        projectsTable->insertRow(row);
        int pid = q.value(0).toInt();
        QString projName = q.value(1).toString();

        projectsTable->setItem(row, 0, new QTableWidgetItem(projName));
        projectsTable->setItem(row, 1, new QTableWidgetItem(q.value(2).toString()));
        projectsTable->setItem(row, 2, new QTableWidgetItem(q.value(3).toString()));
        projectsTable->setItem(row, 3, new QTableWidgetItem(q.value(4).toString()));

        // Status badge
        auto *statusItem = new QTableWidgetItem(q.value(5).toString());
        QString status = q.value(5).toString();
        if (status == "Active" || status == "SEARCHING")
            statusItem->setForeground(QColor("#16a34a"));
        else
            statusItem->setForeground(QColor("#dc2626"));
        projectsTable->setItem(row, 4, statusItem);

        // View Applicants button
        auto *viewBtn = new QPushButton("View Applicants");
        viewBtn->setStyleSheet(primaryBtnStyle());
        viewBtn->setCursor(Qt::PointingHandCursor);
        connect(viewBtn, &QPushButton::clicked, this, [this, pid, projName]
                {
            currentPID = pid;
            activateBtn(btnApplicants);
            stack->setCurrentIndex(2);
            loadApplicantsForProject(pid, projName); });
        projectsTable->setCellWidget(row, 5, viewBtn);
        row++;
    }

    if (row == 0)
    {
        projectsTable->insertRow(0);
        auto *empty = new QTableWidgetItem(
            "No projects posted yet. Use 'Post a Project' to get started!");
        empty->setForeground(QColor("#94a3b8"));
        projectsTable->setItem(0, 0, empty);
    }
}

void TeacherDashboard::addSkillRequirement()
{
    QString name = skillReqNameInput->text().trimmed();
    if (name.isEmpty())
        return;
    int prof = skillReqProfInput->value();
    skillReqList->addItem(name + " | Min Proficiency: " + QString::number(prof));
    skillReqNameInput->clear();
    skillReqProfInput->setValue(5);
}

void TeacherDashboard::removeSkillRequirement()
{
    auto *item = skillReqList->currentItem();
    if (!item)
        return;
    delete skillReqList->takeItem(skillReqList->row(item));
}

void TeacherDashboard::postProject()
{
    if (projNameInput->text().isEmpty() ||
        projDeptInput->text().isEmpty() ||
        projDurationInput->text().isEmpty() ||
        projDescInput->toPlainText().isEmpty())
    {
        postStatusLabel->setText("Please fill in all required fields.");
        return;
    }

    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    // Insert project
    auto q = db.prepareAndExecute(
        "INSERT INTO ProjectDetails "
        "(ProjectName, TID, ProjectDesc, vacantSpot, expectedDuration, "
        "department, preference, applicants, status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, 0, 'Active')",
        {projNameInput->text().trimmed(),
         currentTID,
         projDescInput->toPlainText().trimmed(),
         projSpotsInput->value(),
         projDurationInput->text().trimmed(),
         projDeptInput->text().trimmed(),
         projPrefInput->currentText()});

    // Get the new PID
    auto pidQuery = db.executeQuery("SELECT @@IDENTITY AS PID");
    if (!pidQuery.next())
    {
        postStatusLabel->setText("Project posted but could not retrieve ID.");
        return;
    }
    int newPID = pidQuery.value(0).toInt();

    // Insert skill requirements
    for (int i = 0; i < skillReqList->count(); i++)
    {
        QString itemText = skillReqList->item(i)->text();
        QStringList parts = itemText.split(" | Min Proficiency: ");
        if (parts.size() == 2)
        {
            db.prepareAndExecute(
                "INSERT INTO skillRequirement (PID, skillName, requiredProficiency) "
                "VALUES (?, ?, ?)",
                {newPID, parts[0].trimmed(), parts[1].trimmed().toInt()});
        }
    }

    // Clear form
    projNameInput->clear();
    projDescInput->clear();
    projDeptInput->clear();
    projDurationInput->clear();
    projSpotsInput->setValue(3);
    skillReqList->clear();

    postStatusLabel->setStyleSheet(
        "color: #16a34a; font-size: 12px; background: transparent;");
    postStatusLabel->setText(
        "Project posted successfully! View it in 'My Projects'.");

    QMessageBox::information(this, "Success",
                             "Your project has been posted successfully!");
}

void TeacherDashboard::loadApplicantsForProject(int pid, const QString &projectName)
{
    currentPID = pid;
    applicantsTitle->setText("Applicants for: " + projectName);
    applicantsTable->setRowCount(0);

    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    // Get vacant spot
    auto vsQuery = db.prepareAndExecute(
        "SELECT vacantSpot FROM ProjectDetails WHERE PID = ?", {pid});
    currentVacantSpot = vsQuery.next() ? vsQuery.value(0).toString() : "0";

    // Get applicants
    auto q = db.prepareAndExecute(
        "SELECT a.AID, a.SID, a.Status, a.EngineScore, a.message, "
        "s.Name, s.Program, s.Semester, s.cv_url "
        "FROM Applications a "
        "JOIN StudentDetails s ON a.SID = s.SID "
        "WHERE a.PID = ?",
        {pid});

    int row = 0;
    while (q.next())
    {
        applicantsTable->insertRow(row);

        QString aid = q.value(0).toString();
        QString sid = q.value(1).toString();
        QString status = q.value(2).toString();
        QString score = q.value(3).toString();
        QString message = q.value(4).toString();
        QString name = q.value(5).toString();
        QString program = q.value(6).toString();
        QString sem = q.value(7).toString();
        QString cvUrl = q.value(8).toString();

        applicantsTable->setItem(row, 0, new QTableWidgetItem(name));
        applicantsTable->setItem(row, 1, new QTableWidgetItem(program));
        applicantsTable->setItem(row, 2, new QTableWidgetItem(sem));

        // Score with color
        auto *scoreItem = new QTableWidgetItem(score + "/100");
        scoreItem->setTextAlignment(Qt::AlignCenter);
        int scoreInt = score.toInt();
        if (scoreInt >= 70)
            scoreItem->setForeground(QColor("#16a34a"));
        else if (scoreInt >= 40)
            scoreItem->setForeground(QColor("#d97706"));
        else
            scoreItem->setForeground(QColor("#dc2626"));
        applicantsTable->setItem(row, 3, scoreItem);

        applicantsTable->setItem(row, 4, new QTableWidgetItem(message));

        // CV button
        if (!cvUrl.isEmpty() && cvUrl != "0")
        {
            auto *cvBtn = new QPushButton("View CV");
            cvBtn->setStyleSheet(R"(
                QPushButton {
                    background: #f1f5f9; color: #1e3a8a;
                    border: 1px solid #cbd5e1; border-radius: 4px;
                    padding: 5px 10px; font-size: 12px;
                }
                QPushButton:hover { background: #e2e8f0; }
            )");
            cvBtn->setCursor(Qt::PointingHandCursor);
            connect(cvBtn, &QPushButton::clicked, this, [cvUrl]
                    { QDesktopServices::openUrl(QUrl(cvUrl)); });
            applicantsTable->setCellWidget(row, 5, cvBtn);
        }
        else
        {
            applicantsTable->setItem(row, 5, new QTableWidgetItem("No CV"));
        }

        // Action buttons — only show if still reviewing
        if (status.toLower() == "reviewing")
        {
            auto *actionWidget = new QWidget();
            auto *actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(4, 2, 4, 2);
            actionLayout->setSpacing(4);

            auto *approveBtn = new QPushButton("Approve");
            approveBtn->setStyleSheet(successBtnStyle());
            approveBtn->setCursor(Qt::PointingHandCursor);

            auto *rejectBtn = new QPushButton("Reject");
            rejectBtn->setStyleSheet(dangerBtnStyle());
            rejectBtn->setCursor(Qt::PointingHandCursor);

            connect(approveBtn, &QPushButton::clicked, this,
                    [this, aid, pid, row]
                    {
                        approveApplicant(aid, currentVacantSpot, QString::number(pid), row);
                    });
            connect(rejectBtn, &QPushButton::clicked, this,
                    [this, aid, row]
                    {
                        rejectApplicant(aid, row);
                    });

            actionLayout->addWidget(approveBtn);
            actionLayout->addWidget(rejectBtn);
            applicantsTable->setCellWidget(row, 6, actionWidget);
        }
        else
        {
            // Show status badge
            auto *statusItem = new QTableWidgetItem(status.toUpper());
            statusItem->setTextAlignment(Qt::AlignCenter);
            if (status.toLower() == "approved")
                statusItem->setForeground(QColor("#16a34a"));
            else
                statusItem->setForeground(QColor("#dc2626"));
            applicantsTable->setItem(row, 6, statusItem);
        }

        row++;
    }

    if (row == 0)
    {
        applicantsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No applicants yet for this project.");
        empty->setForeground(QColor("#94a3b8"));
        applicantsTable->setItem(0, 0, empty);
    }
}

void TeacherDashboard::approveApplicant(
    const QString &aid, const QString &vacantSpot,
    const QString &pid, int tableRow)
{
    int vac = vacantSpot.toInt();
    if (vac <= 0)
    {
        QMessageBox::warning(this, "No Spots",
                             "No vacant spots remaining for this project.");
        return;
    }

    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    // Update application status
    db.prepareAndExecute(
        "UPDATE Applications SET Status = 'APPROVED' WHERE AID = ?", {aid});

    // Decrease vacant spot
    db.prepareAndExecute(
        "UPDATE ProjectDetails SET vacantSpot = vacantSpot - 1 WHERE PID = ?",
        {pid.toInt()});

    // Send notification
    Notification n;
    n.createAlert(aid, db.database(), true);

    // Update table UI
    auto *statusItem = new QTableWidgetItem("APPROVED");
    statusItem->setTextAlignment(Qt::AlignCenter);
    statusItem->setForeground(QColor("#16a34a"));
    applicantsTable->setItem(tableRow, 6, statusItem);

    // Update vacant spot count
    currentVacantSpot = QString::number(vac - 1);

    QMessageBox::information(this, "Approved",
                             "Applicant approved! A notification has been sent to the student.");
}

void TeacherDashboard::rejectApplicant(const QString &aid, int tableRow)
{
    auto reply = QMessageBox::question(this, "Confirm Rejection",
                                       "Are you sure you want to reject this applicant?",
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    // Update status
    db.prepareAndExecute(
        "UPDATE Applications SET Status = 'REJECTED' WHERE AID = ?", {aid});

    // Send notification
    Notification n;
    n.createAlert(aid, db.database(), false);

    // Update UI
    auto *statusItem = new QTableWidgetItem("REJECTED");
    statusItem->setTextAlignment(Qt::AlignCenter);
    statusItem->setForeground(QColor("#dc2626"));
    applicantsTable->setItem(tableRow, 6, statusItem);

    QMessageBox::information(this, "Rejected",
                             "Applicant rejected. A notification has been sent to the student.");
}