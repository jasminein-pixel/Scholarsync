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

QWidget *StudentDashboard::buildSkillsScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("My Skills"));
    layout->addWidget(screenSub("Manage your technical skills and proficiency levels."));

    skillsTable = new QTableWidget(0, 3);
    skillsTable->setHorizontalHeaderLabels({"Skill", "Proficiency (1-10)", "Action"});
    skillsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    skillsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    skillsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    skillsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    skillsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    skillsTable->verticalHeader()->hide();
    skillsTable->verticalHeader()->setDefaultSectionSize(38);
    skillsTable->setStyleSheet(StudentStyles::tableStyle());
    skillsTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    skillsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(skillsTable);

    auto *addRow = new QHBoxLayout();
    skillNameInput = new QLineEdit();
    skillNameInput->setPlaceholderText("Skill name (e.g. Python)");
    skillNameInput->setStyleSheet(StudentStyles::inputStyle());
    skillProfInput = new QSpinBox();
    skillProfInput->setRange(1, 10);
    skillProfInput->setValue(5);
    skillProfInput->setStyleSheet(StudentStyles::inputStyle());
    skillProfInput->setFixedWidth(80);

    auto *addBtn = new QPushButton("Add Skill");
    addBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
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

void StudentDashboard::loadSkills()
{
    skillsTable->setUpdatesEnabled(false);
    skillsTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    auto q = db.prepareAndExecute(
        "SELECT SkillName, Proficiency FROM SkillList WHERE SID = ?",
        {currentSID});

    int row = 0;
    while (q.next())
    {
        skillsTable->insertRow(row);
        skillsTable->setItem(row, 0, new QTableWidgetItem(q.value(0).toString()));
        skillsTable->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));

        auto *delBtn = new QPushButton("Remove");
        delBtn->setObjectName("rowDangerBtn");
        delBtn->setCursor(Qt::PointingHandCursor);
        QString skillName = q.value(0).toString();
        connect(delBtn, &QPushButton::clicked, this, [this, skillName]
        {
            auto &db = DatabaseManager::instance();
            db.prepareAndExecute(
                "DELETE FROM SkillList WHERE SID = ? AND SkillName = ?",
                {currentSID, skillName});
            loadSkills();
        });
        skillsTable->setCellWidget(row, 2, delBtn);
        row++;
    }

    if (row == 0)
    {
        skillsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No skills added yet. Add one below!");
        empty->setForeground(QColor("#94a3b8"));
        skillsTable->setItem(0, 0, empty);
    }

    adjustTableHeight(skillsTable, row);
    skillsTable->setUpdatesEnabled(true);
}

void StudentDashboard::addSkill()
{
    QString name = skillNameInput->text().trimmed();
    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Missing", "Please enter a skill name.");
        return;
    }
    int prof = skillProfInput->value();

    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    auto check = db.prepareAndExecute(
        "SELECT COUNT(*) FROM SkillList WHERE SID = ? AND SkillName = ?",
        {currentSID, name});
    check.next();
    if (check.value(0).toInt() > 0)
    {
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
    if (row < 0)
    {
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
