#include "StudentDashboard.h"
#include "StudentStyles.h"
#include "PieChartWidget.h"
#include "../../database/DatabaseManager.h"
#include <QDesktopServices>
#include <QUrl>
#include <QIntValidator>

extern int currentSID;
extern QString currentUserName;

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

static QWidget *profileInfoField(const QString &caption, QLabel *&valueLabelOut)
{
    auto *w = new QWidget();
    w->setStyleSheet("background: transparent;");
    auto *l = new QVBoxLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(2);

    auto *captionLbl = new QLabel(caption);
    captionLbl->setStyleSheet(
        "font-size: 11px; font-weight: 700; color: #64748b; "
        "letter-spacing: 0.5px; background: transparent;");

    valueLabelOut = new QLabel("—");
    valueLabelOut->setStyleSheet(
        "font-size: 14px; color: #0f172a; background: transparent;");

    l->addWidget(captionLbl);
    l->addWidget(valueLabelOut);
    return w;
}

static QWidget *profileInfoFieldEditable(const QString &caption,
                                         QLabel *&valueLabelOut,
                                         QLineEdit *&editOut)
{
    auto *w = new QWidget();
    w->setStyleSheet("background: transparent;");
    auto *l = new QVBoxLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(2);

    auto *captionLbl = new QLabel(caption);
    captionLbl->setStyleSheet(
        "font-size: 11px; font-weight: 700; color: #64748b; "
        "letter-spacing: 0.5px; background: transparent;");

    valueLabelOut = new QLabel("—");
    valueLabelOut->setStyleSheet(
        "font-size: 14px; color: #0f172a; background: transparent;");

    editOut = new QLineEdit();
    editOut->setStyleSheet(R"(
        QLineEdit {
            border: 1.5px solid #cbd5e1;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            background: white;
            color: #1e293b;
            min-height: 36px;
        }
        QLineEdit:focus { border-color: #3b82f6; }
    )");
    editOut->hide();

    l->addWidget(captionLbl);
    l->addWidget(valueLabelOut);
    l->addWidget(editOut);
    return w;
}

static QLabel *profileSectionHeader(const QString &text)
{
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 14px; font-weight: 700; color: #0f172a; "
        "background: transparent; margin-top: 12px; margin-bottom: 4px;");
    return lbl;
}

static QString prettyStatus(const QString &raw)
{
    QString s = raw.trimmed().toLower();
    if (s.isEmpty())
        return "Unknown";
    if (s == "pending")
        s = "reviewing";
    s[0] = s[0].toUpper();
    return s;
}

QWidget *StudentDashboard::buildProfileScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");

    auto *outer = new QVBoxLayout(w);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: #f8fafc; }");

    auto *content = new QWidget();
    content->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(8);

    layout->addWidget(screenTitle("My Profile"));
    layout->addWidget(screenSub("Your account details, credits, and project history."));

    auto *infoCard = new QWidget();
    infoCard->setStyleSheet(R"(
        QWidget {
            background: white;
            border-radius: 8px;
        }
    )");
    auto *infoGrid = new QVBoxLayout(infoCard);
    infoGrid->setContentsMargins(20, 16, 20, 16);
    infoGrid->setSpacing(12);

    auto *nameRow = new QHBoxLayout();
    profileNameLabel = new QLabel(currentUserName.isEmpty() ? "Student" : currentUserName);
    profileNameLabel->setStyleSheet(
        "font-size: 17px; font-weight: 700; color: #0f172a; background: transparent;");
    profileEditSaveBtn = new QPushButton("Edit Details");
    profileEditSaveBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #1d4ed8;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 5px 12px;
            font-size: 11px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #1e40af; }
    )");
    profileEditSaveBtn->setCursor(Qt::PointingHandCursor);
    profileEditSaveBtn->setMaximumHeight(26);
    nameRow->addWidget(profileNameLabel);
    nameRow->addStretch();
    nameRow->addWidget(profileEditSaveBtn);
    infoGrid->addLayout(nameRow);

    auto *fieldsGrid = new QGridLayout();
    fieldsGrid->setHorizontalSpacing(28);
    fieldsGrid->setVerticalSpacing(14);

    fieldsGrid->addWidget(profileInfoField("EMAIL", profileEmailLabel), 0, 0);
    fieldsGrid->addWidget(
        profileInfoFieldEditable("SEMESTER", profileSemesterLabel, profileSemesterEdit), 0, 1);
    fieldsGrid->addWidget(profileInfoField("PROGRAM", profileProgramLabel), 0, 2);
    fieldsGrid->addWidget(profileInfoField("DEPARTMENT", profileDepartmentLabel), 0, 3);

    fieldsGrid->addWidget(profileInfoField("LEVEL", profileLevelLabel), 1, 0);
    fieldsGrid->addWidget(profileInfoField("PREFERENCE", profilePreferenceLabel), 1, 1);
    fieldsGrid->addWidget(profileInfoField("CREDITS", profileCreditsLabel), 1, 2);
    fieldsGrid->addWidget(
        profileInfoFieldEditable("CONTACT INFO", profileContactLabel, profileContactEdit), 1, 3);

    infoGrid->addLayout(fieldsGrid);

    profileSemesterEdit->setValidator(new QIntValidator(1, 20, profileSemesterEdit));
    profileContactEdit->setPlaceholderText("Phone number");

    profileCvStatusBtn = new QPushButton("");
    profileCvStatusBtn->setCursor(Qt::PointingHandCursor);
    profileCvStatusBtn->setStyleSheet(R"(
        QPushButton {
            text-align: left;
            background: transparent;
            border: none;
            color: #1d4ed8;
            font-size: 13px;
            font-weight: 600;
            padding: 6px 0 0;
        }
        QPushButton:hover { color: #1e40af; text-decoration: underline; }
    )");
    connect(profileCvStatusBtn, &QPushButton::clicked, this, [this]
    {
        if (!profileCvUrl.isEmpty() && profileCvUrl != "0") {
            QDesktopServices::openUrl(QUrl(profileCvUrl));
        } else {
            stack->setCurrentIndex(3);
            activateBtn(btnCV);
        }
    });
    infoGrid->addWidget(profileCvStatusBtn);

    layout->addWidget(infoCard);

    layout->addWidget(profileSectionHeader("My Skills"));
    profileSkillsTable = new QTableWidget(0, 2);
    profileSkillsTable->setHorizontalHeaderLabels({"Skill", "Proficiency"});
    profileSkillsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profileSkillsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profileSkillsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileSkillsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profileSkillsTable->verticalHeader()->hide();
    profileSkillsTable->verticalHeader()->setDefaultSectionSize(38);
    profileSkillsTable->setStyleSheet(StudentStyles::tableStyle());
    profileSkillsTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    profileSkillsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(profileSkillsTable);

    layout->addWidget(profileSectionHeader("My Interests"));
    profileInterestsTable = new QTableWidget(0, 1);
    profileInterestsTable->setHorizontalHeaderLabels({"Interest"});
    profileInterestsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profileInterestsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileInterestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profileInterestsTable->verticalHeader()->hide();
    profileInterestsTable->verticalHeader()->setDefaultSectionSize(38);
    profileInterestsTable->setStyleSheet(StudentStyles::tableStyle());
    profileInterestsTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    profileInterestsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(profileInterestsTable);

    connect(profileEditSaveBtn, &QPushButton::clicked, this, [this]
    {
        if (!profileEditMode) {
            profileSemesterEdit->setText(
                profileSemesterLabel->text() == "—" ? "" : profileSemesterLabel->text());
            profileSemesterLabel->hide();
            profileSemesterEdit->show();

            profileContactEdit->setText(
                profileContactLabel->text() == "—" ? "" : profileContactLabel->text());
            profileContactLabel->hide();
            profileContactEdit->show();

            profileEditSaveBtn->setText("Save Changes");
            profileEditMode = true;
        } else {
            auto &db = DatabaseManager::instance();
            QString semesterVal = profileSemesterEdit->text().trimmed();
            QString contactVal  = profileContactEdit->text().trimmed();

            db.prepareAndExecute(
                "UPDATE StudentDetails SET Semester = ?, ContactInfo = ? WHERE SID = ?",
                {semesterVal, contactVal, currentSID});

            profileEditSaveBtn->setText("Edit Details");
            profileEditMode = false;
            loadProfile();
        }
    });

    layout->addWidget(profileSectionHeader("Applications"));
    profileApplicationsTable = new QTableWidget(0, 4);
    profileApplicationsTable->setHorizontalHeaderLabels({"Project Name", "Department", "Status", "Score"});
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    profileApplicationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileApplicationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profileApplicationsTable->verticalHeader()->hide();
    profileApplicationsTable->verticalHeader()->setDefaultSectionSize(38);
    profileApplicationsTable->setStyleSheet(StudentStyles::tableStyle());
    profileApplicationsTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    profileApplicationsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(profileApplicationsTable);

    layout->addWidget(profileSectionHeader("Current Projects"));
    profileCurrentTable = new QTableWidget(0, 4);
    profileCurrentTable->setHorizontalHeaderLabels({"Project Name", "Department", "Duration", "Supervisor"});
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    profileCurrentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileCurrentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profileCurrentTable->verticalHeader()->hide();
    profileCurrentTable->verticalHeader()->setDefaultSectionSize(38);
    profileCurrentTable->setStyleSheet(StudentStyles::tableStyle());
    profileCurrentTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    profileCurrentTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(profileCurrentTable);

    layout->addWidget(profileSectionHeader("Past Projects"));
    profilePastTable = new QTableWidget(0, 2);
    profilePastTable->setHorizontalHeaderLabels({"Project Name", "Department"});
    profilePastTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profilePastTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profilePastTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profilePastTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profilePastTable->verticalHeader()->hide();
    profilePastTable->verticalHeader()->setDefaultSectionSize(38);
    profilePastTable->setStyleSheet(StudentStyles::tableStyle());
    profilePastTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    profilePastTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(profilePastTable);

    layout->addWidget(profileSectionHeader("Application History (Lifetime)"));
    profileAppsChart = new PieChartWidget();
    profileAppsChart->setStyleSheet("background: white; border-radius: 8px;");
    layout->addWidget(profileAppsChart);

    layout->addStretch();

    scrollArea->setWidget(content);
    outer->addWidget(scrollArea);

    return w;
}

void StudentDashboard::loadProfile()
{
    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    profileSkillsTable->setUpdatesEnabled(false);
    profileInterestsTable->setUpdatesEnabled(false);
    profileApplicationsTable->setUpdatesEnabled(false);
    profileCurrentTable->setUpdatesEnabled(false);
    profilePastTable->setUpdatesEnabled(false);

    profileSemesterLabel->show();
    profileSemesterEdit->hide();
    profileContactLabel->show();
    profileContactEdit->hide();
    profileEditMode = false;
    profileEditSaveBtn->setText("Edit Details");

    profileNameLabel->setText(currentUserName.isEmpty() ? "Student" : currentUserName);

    auto textOr = [](const QVariant &v) -> QString
    {
        QString s = v.toString().trimmed();
        return s.isEmpty() ? QStringLiteral("—") : s;
    };

    auto infoQ = db.prepareAndExecute(
        "SELECT Email, Semester, Program, Department, Level, Preference, Credit, ContactInfo, cv_url "
        "FROM StudentDetails WHERE SID = ?",
        {currentSID});

    if (infoQ.next())
    {
        profileEmailLabel->setText(textOr(infoQ.value(0)));
        profileSemesterLabel->setText(textOr(infoQ.value(1)));
        profileProgramLabel->setText(textOr(infoQ.value(2)));
        profileDepartmentLabel->setText(textOr(infoQ.value(3)));
        profileLevelLabel->setText(textOr(infoQ.value(4)));
        profilePreferenceLabel->setText(textOr(infoQ.value(5)));
        profileCreditsLabel->setText(textOr(infoQ.value(6)));
        profileContactLabel->setText(textOr(infoQ.value(7)));

        profileCvUrl = infoQ.value(8).toString().trimmed();
        bool hasCv = !(profileCvUrl.isEmpty() || profileCvUrl == "0");
        profileCvStatusBtn->setText(hasCv
                                        ? "📄  View My CV"
                                        : "⚠️  No CV uploaded yet — click to add one");
    }
    else
    {
        profileEmailLabel->setText("—");
        profileSemesterLabel->setText("—");
        profileProgramLabel->setText("—");
        profileDepartmentLabel->setText("—");
        profileLevelLabel->setText("—");
        profilePreferenceLabel->setText("—");
        profileCreditsLabel->setText("—");
        profileContactLabel->setText("—");
        profileCvUrl.clear();
        profileCvStatusBtn->setText("⚠️  No CV uploaded yet — click to add one");
    }

    profileSkillsTable->setRowCount(0);
    auto skillsQ = db.prepareAndExecute(
        "SELECT SkillName, Proficiency FROM SkillList WHERE SID = ? ORDER BY Proficiency DESC",
        {currentSID});

    int row = 0;
    while (skillsQ.next())
    {
        profileSkillsTable->insertRow(row);
        profileSkillsTable->setItem(row, 0,
                                    new QTableWidgetItem(skillsQ.value(0).toString()));
        profileSkillsTable->setItem(row, 1,
                                    new QTableWidgetItem(skillsQ.value(1).toString() + " / 10"));
        row++;
    }
    if (row == 0)
    {
        profileSkillsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No skills added yet.");
        empty->setForeground(QColor("#94a3b8"));
        profileSkillsTable->setItem(0, 0, empty);
    }
    adjustTableHeight(profileSkillsTable, row);

    profileInterestsTable->setRowCount(0);
    auto interestsQ = db.prepareAndExecute(
        "SELECT Interest FROM Interest WHERE SID = ? ORDER BY Interest ASC",
        {currentSID});

    row = 0;
    while (interestsQ.next())
    {
        profileInterestsTable->insertRow(row);
        profileInterestsTable->setItem(row, 0,
                                       new QTableWidgetItem(interestsQ.value(0).toString()));
        row++;
    }
    if (row == 0)
    {
        profileInterestsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No interests added yet.");
        empty->setForeground(QColor("#94a3b8"));
        profileInterestsTable->setItem(0, 0, empty);
    }
    adjustTableHeight(profileInterestsTable, row);

    profileApplicationsTable->setRowCount(0);
    auto appQ = db.prepareAndExecute(
        "SELECT p.ProjectName, p.department, a.Status, a.EngineScore "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "WHERE a.SID = ? "
        "ORDER BY a.AppliedAt DESC",
        {currentSID});

    row = 0;
    while (appQ.next())
    {
        profileApplicationsTable->insertRow(row);
        profileApplicationsTable->setItem(row, 0,
                                          new QTableWidgetItem(appQ.value(0).toString()));
        profileApplicationsTable->setItem(row, 1,
                                          new QTableWidgetItem(appQ.value(1).toString()));

        QString rawStatus = appQ.value(2).toString();
        auto *statusItem = new QTableWidgetItem(prettyStatus(rawStatus));
        QString s = rawStatus.trimmed().toLower();
        if (s == "accepted")
            statusItem->setForeground(QColor("#16a34a"));
        else if (s == "rejected")
            statusItem->setForeground(QColor("#dc2626"));
        else
            statusItem->setForeground(QColor("#d97706"));
        profileApplicationsTable->setItem(row, 2, statusItem);

        profileApplicationsTable->setItem(row, 3,
                                          new QTableWidgetItem(appQ.value(3).toString()));
        row++;
    }
    if (row == 0)
    {
        profileApplicationsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No applications submitted yet.");
        empty->setForeground(QColor("#94a3b8"));
        profileApplicationsTable->setItem(0, 0, empty);
    }
    adjustTableHeight(profileApplicationsTable, row);

    profileCurrentTable->setRowCount(0);
    auto curQ = db.prepareAndExecute(
        "SELECT p.ProjectName, p.department, p.expectedDuration, t.Name "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "LEFT JOIN TeacherDetails t ON p.TID = t.TID "
        "WHERE a.SID = ? AND LOWER(a.Status) = 'accepted' AND LOWER(p.status) = 'active'",
        {currentSID});

    row = 0;
    while (curQ.next())
    {
        profileCurrentTable->insertRow(row);
        profileCurrentTable->setItem(row, 0,
                                     new QTableWidgetItem(curQ.value(0).toString()));
        profileCurrentTable->setItem(row, 1,
                                     new QTableWidgetItem(curQ.value(1).toString()));
        profileCurrentTable->setItem(row, 2,
                                     new QTableWidgetItem(curQ.value(2).toString()));
        QString supervisor = curQ.value(3).toString().trimmed();
        profileCurrentTable->setItem(row, 3,
                                     new QTableWidgetItem(supervisor.isEmpty() ? "—" : supervisor));
        row++;
    }
    if (row == 0)
    {
        profileCurrentTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No current projects.");
        empty->setForeground(QColor("#94a3b8"));
        profileCurrentTable->setItem(0, 0, empty);
    }
    adjustTableHeight(profileCurrentTable, row);

    profilePastTable->setRowCount(0);
    auto pastQ = db.prepareAndExecute(
        "SELECT p.ProjectName, p.department "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "WHERE a.SID = ? AND LOWER(a.Status) = 'accepted' AND LOWER(p.status) = 'completed'",
        {currentSID});

    row = 0;
    while (pastQ.next())
    {
        profilePastTable->insertRow(row);
        profilePastTable->setItem(row, 0,
                                  new QTableWidgetItem(pastQ.value(0).toString()));
        profilePastTable->setItem(row, 1,
                                  new QTableWidgetItem(pastQ.value(1).toString()));
        row++;
    }
    if (row == 0)
    {
        profilePastTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No past projects.");
        empty->setForeground(QColor("#94a3b8"));
        profilePastTable->setItem(0, 0, empty);
    }
    adjustTableHeight(profilePastTable, row);

    auto histQ = db.prepareAndExecute(
        "SELECT LOWER(a.Status), LOWER(p.status), COUNT(*) "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "WHERE a.SID = ? "
        "GROUP BY LOWER(a.Status), LOWER(p.status)",
        {currentSID});

    int cReviewing = 0, cRejected = 0, cActive = 0, cCompleted = 0;
    while (histQ.next())
    {
        QString appStatus = histQ.value(0).toString().trimmed().toLower();
        QString projStatus = histQ.value(1).toString().trimmed().toLower();
        int cnt = histQ.value(2).toInt();

        if (appStatus == "reviewing" || appStatus == "pending")
        {
            cReviewing += cnt;
        }
        else if (appStatus == "rejected")
        {
            cRejected += cnt;
        }
        else if (appStatus == "accepted")
        {
            if (projStatus == "completed")
                cCompleted += cnt;
            else
                cActive += cnt;
        }
    }

    profileAppsChart->setData({
        {"Reviewing", cReviewing, QColor("#93c5fd")},
        {"Active", cActive, QColor("#3b82f6")},
        {"Completed", cCompleted, QColor("#1e3a8a")},
        {"Rejected", cRejected, QColor("#ef4444")}
    });

    profileSkillsTable->setUpdatesEnabled(true);
    profileInterestsTable->setUpdatesEnabled(true);
    profileApplicationsTable->setUpdatesEnabled(true);
    profileCurrentTable->setUpdatesEnabled(true);
    profilePastTable->setUpdatesEnabled(true);
}
