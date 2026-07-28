#include "StudentDashboard.h"
#include "StudentStyles.h"
#include "../../database/DatabaseManager.h"

extern int currentSID;

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
        "font-size: 13px; color: #64748b; background: transparent; margin-bottom: 16px;");
    return lbl;
}

QWidget *StudentDashboard::buildBrowseScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(4);

    layout->addWidget(screenTitle("Browse Projects"));
    layout->addWidget(screenSub("Explore active research projects available for application."));

    layout->addSpacing(12);

    projectsTable = new QTableWidget(0, 6);
    projectsTable->setHorizontalHeaderLabels({"Project Name", "Department", "Duration", "Spots", "Preference", "Action"});
    projectsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    projectsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    projectsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    projectsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    projectsTable->verticalHeader()->hide();
    projectsTable->setStyleSheet(StudentStyles::tableStyle());
    layout->addWidget(projectsTable);

    layout->addSpacing(12);

    auto *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(0);
    auto *refreshBtn = new QPushButton("Refresh");
    refreshBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &StudentDashboard::loadProjects);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    layout->addStretch();

    return w;
}

void StudentDashboard::loadProjects()
{
    projectsTable->setUpdatesEnabled(false);
    projectsTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    auto q = db.executeQuery(
        "SELECT PID, ProjectName, department, expectedDuration, vacantSpot, preference "
        "FROM ProjectDetails WHERE status = 'Active' AND vacantSpot > 0");

    int row = 0;
    while (q.next())
    {
        projectsTable->insertRow(row);
        int pid = q.value(0).toInt();
        projectsTable->setItem(row, 0, new QTableWidgetItem(q.value(1).toString()));
        projectsTable->setItem(row, 1, new QTableWidgetItem(q.value(2).toString()));
        projectsTable->setItem(row, 2, new QTableWidgetItem(q.value(3).toString()));
        projectsTable->setItem(row, 3, new QTableWidgetItem(q.value(4).toString()));
        projectsTable->setItem(row, 4, new QTableWidgetItem(q.value(5).toString()));

        auto *applyBtn = new QPushButton("Apply");
        applyBtn->setObjectName("rowPrimaryBtn");
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this, pid]
        {
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
                                     "Your application has been submitted successfully.");
            loadProjects();
        });
        projectsTable->setCellWidget(row, 5, applyBtn);
        row++;
    }

    if (row == 0)
    {
        projectsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No active projects available right now.");
        empty->setForeground(QColor("#94a3b8"));
        projectsTable->setItem(0, 0, empty);
    }

    adjustTableHeight(projectsTable, row);
    projectsTable->setUpdatesEnabled(true);
}
