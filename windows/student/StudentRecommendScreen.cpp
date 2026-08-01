#include "../student/StudentDashboard.h"
#include "../student/StudentStyles.h"
#include "../../database/DatabaseManager.h"
#include "../../services/MatchingEngine.h"
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>

extern int currentSID;

QWidget* StudentDashboard::buildRecommendScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    auto *title = new QLabel("Recommendations");
    title->setStyleSheet(
        "font-size: 20px; font-weight: 700; color: #0f172a; "
        "background: transparent;");
    layout->addWidget(title);

    auto *sub = new QLabel(
        "Projects matched to your skills and interests, ranked by score.");
    sub->setStyleSheet(
        "font-size: 13px; color: #64748b; background: transparent;");
    layout->addWidget(sub);

    // Refresh button — forces recalculation
    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("Refresh");
    refreshBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, [this](){
        recommendationsLoaded = false;
        cachedRecommendations.clear();
        loadRecommendations();
    });
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    recommendTable = new QTableWidget(0, 4);
    recommendTable->setHorizontalHeaderLabels({
        "Project Name", "Department", "Match Score", "Action"});
    recommendTable->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    recommendTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(
        3, QHeaderView::ResizeToContents);
    recommendTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    recommendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recommendTable->verticalHeader()->hide();
    recommendTable->setStyleSheet(StudentStyles::tableStyle());
    layout->addWidget(recommendTable);

    return w;
}

void StudentDashboard::loadRecommendations()
{
    // ── If already calculated this session, just return ──
    if (recommendationsLoaded && recommendTable->rowCount() > 0)
        return;

    recommendTable->setRowCount(0);

    auto &dbm = DatabaseManager::instance();
    if (!dbm.isConnected()) dbm.connect();
    QSqlDatabase db = dbm.database();

    // ── Show loading message while engine runs ──
    recommendTable->insertRow(0);
    recommendTable->setItem(0, 0,
        new QTableWidgetItem("Calculating matches... please wait"));
    QApplication::processEvents();  // forces UI to refresh and show message

    // ── Run matching engine ──
    MatchingEngine engine;
    cachedRecommendations = engine.getRankedProjects(currentSID, db);

    // ── Clear loading message ──
    recommendTable->setRowCount(0);

    // ── Handle empty results ──
    if (cachedRecommendations.isEmpty()) {
        recommendTable->insertRow(0);
        auto *empty = new QTableWidgetItem(
            "No matches found. Try adding more skills!");
        empty->setForeground(QColor("#94a3b8"));
        recommendTable->setItem(0, 0, empty);
        recommendationsLoaded = true;
        return;
    }

    // ── Display cached results ──
    for (auto &[score, pid] : cachedRecommendations) {
        auto q = dbm.prepareAndExecute(
            "SELECT ProjectName, department FROM ProjectDetails WHERE PID = ?",
            {pid});
        if (!q.next()) continue;

        int row = recommendTable->rowCount();
        recommendTable->insertRow(row);

        recommendTable->setItem(row, 0,
            new QTableWidgetItem(q.value(0).toString()));
        recommendTable->setItem(row, 1,
            new QTableWidgetItem(q.value(1).toString()));

        // Score with color coding
        auto *scoreItem = new QTableWidgetItem(
            QString::number(score) + "/10");
        scoreItem->setTextAlignment(Qt::AlignCenter);
        if      (score >= 7) scoreItem->setForeground(QColor("#16a34a")); // green
        else if (score >= 4) scoreItem->setForeground(QColor("#d97706")); // orange
        else                 scoreItem->setForeground(QColor("#dc2626")); // red
        recommendTable->setItem(row, 2, scoreItem);

        // Apply button
        auto *applyBtn = new QPushButton("Apply");
        applyBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this, pid, score]() {
            auto &dbm = DatabaseManager::instance();
            QSqlDatabase db = dbm.database();

            // Check already applied
            auto check = dbm.prepareAndExecute(
                "SELECT COUNT(*) FROM Applications WHERE SID = ? AND PID = ?",
                {currentSID, pid});
            check.next();
            if (check.value(0).toInt() > 0) {
                QMessageBox::information(this, "Already Applied",
                    "You have already applied to this project.");
                return;
            }

            // Insert with the cached score
            dbm.prepareAndExecute(
                "INSERT INTO Applications "
                "(SID, PID, Status, EngineScore, message) "
                "VALUES (?, ?, 'reviewing', ?, "
                "'Applied via Recommendations')",
                {currentSID, pid, score});

            QMessageBox::information(this, "Applied!",
                QString("Application submitted!\nMatch score: %1/10")
                .arg(score));
        });
        recommendTable->setCellWidget(row, 3, applyBtn);
    }

    recommendationsLoaded = true;  // mark as loaded for this session
}