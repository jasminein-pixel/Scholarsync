#include "StudentDashboard.h"
#include "StudentStyles.h"
#include "../../database/DatabaseManager.h"
#include "../../services/MatchingEngine.h"

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

QWidget *StudentDashboard::buildRecommendScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(4);

    layout->addWidget(screenTitle("Recommendations"));
    layout->addWidget(screenSub(
        "Projects recommended based on your skills and interests, ranked by match score."));

    layout->addSpacing(12);

    recommendTable = new QTableWidget(0, 4);
    recommendTable->setHorizontalHeaderLabels({"Project Name", "Department", "Match Score", "Action"});
    recommendTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    recommendTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    recommendTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    recommendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recommendTable->verticalHeader()->hide();
    recommendTable->verticalHeader()->setDefaultSectionSize(38);
    recommendTable->setStyleSheet(StudentStyles::tableStyle());
    recommendTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    recommendTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(recommendTable);

    layout->addSpacing(12);

    auto *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(0);
    auto *refreshBtn = new QPushButton("Refresh");
    refreshBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked,
            this, &StudentDashboard::loadRecommendations);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    layout->addStretch();

    return w;
}

void StudentDashboard::loadRecommendations()
{
    recommendTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    QSqlDatabase database = db.database();

    MatchingEngine engine;
    auto ranked = engine.getRankedProjects(currentSID, database);

    for (auto &[score, pid] : ranked) {
        QSqlQuery q(database);
        q.prepare("SELECT ProjectName, department FROM ProjectDetails WHERE PID = ?");
        q.addBindValue(pid);
        if (!q.exec() || !q.next()) continue;

        int row = recommendTable->rowCount();
        recommendTable->insertRow(row);
        recommendTable->setItem(row, 0, new QTableWidgetItem(q.value(0).toString()));
        recommendTable->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));

        QString scoreText = QString::number(score) + "/10";
        auto *scoreItem = new QTableWidgetItem(scoreText);
        scoreItem->setTextAlignment(Qt::AlignCenter);
        if (score >= 7)      scoreItem->setForeground(QColor("#16a34a"));
        else if (score >= 4) scoreItem->setForeground(QColor("#d97706"));
        else                 scoreItem->setForeground(QColor("#dc2626"));
        recommendTable->setItem(row, 2, scoreItem);

        auto *applyBtn = new QPushButton("Apply");
        applyBtn->setFixedWidth(80);
        applyBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this, pid, score]{
            auto &db = DatabaseManager::instance();
            QSqlDatabase database = db.database();

            QSqlQuery check(database);
            check.prepare("SELECT COUNT(*) FROM Applications WHERE SID = ? AND PID = ?");
            check.addBindValue(currentSID);
            check.addBindValue(pid);
            check.exec();
            check.next();
            if (check.value(0).toInt() > 0) {
                QMessageBox::information(this, "Already Applied",
                    "You have already applied to this project.");
                return;
            }

            QSqlQuery insert(database);
            insert.prepare(
                "INSERT INTO Applications "
                "(SID, PID, Status, EngineScore, message) "
                "VALUES (?, ?, 'reviewing', ?, "
                "'Application submitted via ScholarSync')");
            insert.addBindValue(currentSID);
            insert.addBindValue(pid);
            insert.addBindValue(score * 10);
            insert.exec();

            QMessageBox::information(this, "Applied!",
                "Application submitted successfully!");
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
