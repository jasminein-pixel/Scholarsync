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

    auto addField = [&](const QString &label, QLineEdit *&field,
                        const QString &placeholder)
    {
        auto *lbl = new QLabel(label);
        lbl->setStyleSheet(
            "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
        field = new QLineEdit();
        field->setPlaceholderText(placeholder);
        field->setStyleSheet(TeacherStyles::inputStyle());
        layout->addWidget(lbl);
        layout->addWidget(field);
    };

    addField("Project Name", projNameInput, "e.g. AI-powered Medical Diagnosis");
    addField("Department", projDeptInput, "e.g. Department Of Computer Science");
    addField("Duration", projDurationInput, "e.g. 3 Months");

    auto *descLabel = new QLabel("Project Description");
    descLabel->setStyleSheet(
        "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
    projDescInput = new QTextEdit();
    projDescInput->setPlaceholderText("Describe the project goals, methodology...");
    projDescInput->setStyleSheet(TeacherStyles::inputStyle());
    projDescInput->setMinimumHeight(100);
    layout->addWidget(descLabel);
    layout->addWidget(projDescInput);

    auto *row1 = new QHBoxLayout();
    auto *spotsLabel = new QLabel("Vacant Spots");
    spotsLabel->setStyleSheet(
        "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
    projSpotsInput = new QSpinBox();
    projSpotsInput->setRange(1, 20);
    projSpotsInput->setValue(3);
    projSpotsInput->setStyleSheet(TeacherStyles::inputStyle());

    auto *prefLabel = new QLabel("Preference");
    prefLabel->setStyleSheet(
        "font-size: 12px; font-weight: 600; color: #16213e; background: transparent;");
    projPrefInput = new QComboBox();
    projPrefInput->addItems({"On-Site", "Remote", "Hybrid"});
    projPrefInput->setStyleSheet(TeacherStyles::inputStyle());

    auto *col1 = new QVBoxLayout();
    col1->addWidget(spotsLabel);
    col1->addWidget(projSpotsInput);
    auto *col2 = new QVBoxLayout();
    col2->addWidget(prefLabel);
    col2->addWidget(projPrefInput);
    row1->addLayout(col1);
    row1->addLayout(col2);
    layout->addLayout(row1);

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

    auto *skillRow = new QHBoxLayout();
    skillReqNameInput = new QLineEdit();
    skillReqNameInput->setPlaceholderText("Skill name (e.g. Python)");
    skillReqNameInput->setStyleSheet(TeacherStyles::inputStyle());

    skillReqProfInput = new QSpinBox();
    skillReqProfInput->setRange(1, 10);
    skillReqProfInput->setValue(5);
    skillReqProfInput->setStyleSheet(TeacherStyles::inputStyle());
    skillReqProfInput->setFixedWidth(80);

    auto *addSkillBtn = new QPushButton("Add");
    addSkillBtn->setStyleSheet(TeacherStyles::primaryBtnStyle());
    addSkillBtn->setCursor(Qt::PointingHandCursor);
    connect(addSkillBtn, &QPushButton::clicked,
            this, &TeacherDashboard::addSkillRequirement);

    auto *removeSkillBtn = new QPushButton("Remove");
    removeSkillBtn->setStyleSheet(TeacherStyles::dangerBtnStyle());
    removeSkillBtn->setCursor(Qt::PointingHandCursor);
    connect(removeSkillBtn, &QPushButton::clicked,
            this, &TeacherDashboard::removeSkillRequirement);

    skillRow->addWidget(skillReqNameInput);
    skillRow->addWidget(new QLabel("Min:"));
    skillRow->addWidget(skillReqProfInput);
    skillRow->addWidget(addSkillBtn);
    skillRow->addWidget(removeSkillBtn);
    layout->addLayout(skillRow);

    postStatusLabel = new QLabel("");
    postStatusLabel->setStyleSheet(
        "color: #dc2626; font-size: 12px; background: transparent;");
    postStatusLabel->setWordWrap(true);
    layout->addWidget(postStatusLabel);

    auto *postBtn = new QPushButton("Post Project");
    postBtn->setStyleSheet(TeacherStyles::primaryBtnStyle());
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

    auto pidQuery = db.executeQuery("SELECT @@IDENTITY AS PID");
    if (!pidQuery.next())
    {
        postStatusLabel->setText("Project posted but could not retrieve ID.");
        return;
    }
    int newPID = pidQuery.value(0).toInt();

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
