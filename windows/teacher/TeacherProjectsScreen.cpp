#include "TeacherDashboard.h"
#include "TeacherStyles.h"
#include "../../database/DatabaseManager.h"

extern int currentTID;

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
    refreshBtn->setStyleSheet(TeacherStyles::primaryBtnStyle());
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
    projectsTable->setStyleSheet(TeacherStyles::tableStyle());
    layout->addWidget(projectsTable);

    return w;
}

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

        auto *statusItem = new QTableWidgetItem(q.value(5).toString());
        QString status = q.value(5).toString();
        if (status == "Active" || status == "SEARCHING")
            statusItem->setForeground(QColor("#16a34a"));
        else
            statusItem->setForeground(QColor("#dc2626"));
        projectsTable->setItem(row, 4, statusItem);

        auto *viewBtn = new QPushButton("View Applicants");
        viewBtn->setStyleSheet(TeacherStyles::primaryBtnStyle());
        viewBtn->setCursor(Qt::PointingHandCursor);
        connect(viewBtn, &QPushButton::clicked, this, [this, pid, projName]
        {
            currentPID = pid;
            activateBtn(btnApplicants);
            stack->setCurrentIndex(2);
            loadApplicantsForProject(pid, projName);
        });
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
