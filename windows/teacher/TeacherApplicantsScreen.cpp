#include "TeacherDashboard.h"
#include "TeacherStyles.h"
#include "../../database/DatabaseManager.h"
#include "../../services/MatchingEngine.h"
#include "../../services/notification.h"
#include <QDesktopServices>
#include <QUrl>
#include <algorithm>

static QLabel *screenSub(const QString &text)
{
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 13px; color: #64748b; background: transparent; margin-bottom: 12px;");
    return lbl;
}

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
    applicantsTable->setStyleSheet(TeacherStyles::tableStyle());
    layout->addWidget(applicantsTable);

    return w;
}

void TeacherDashboard::loadApplicantsForProject(int pid, const QString &projectName)
{
    currentPID = pid;
    applicantsTitle->setText("Applicants for: " + projectName);
    applicantsTable->setRowCount(0);

    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();
    QSqlDatabase database = db.database();

    QSqlQuery vsQuery(database);
    vsQuery.prepare("SELECT vacantSpot FROM ProjectDetails WHERE PID = ?");
    vsQuery.addBindValue(pid);
    vsQuery.exec();
    currentVacantSpot = vsQuery.next() ? vsQuery.value(0).toString() : "0";

    QSqlQuery q(database);
    q.prepare(
        "SELECT a.AID, a.SID, a.Status, a.EngineScore, a.message, "
        "s.Name, s.Program, s.Semester, s.cv_url "
        "FROM Applications a "
        "JOIN StudentDetails s ON a.SID = s.SID "
        "WHERE a.PID = ?");
    q.addBindValue(pid);
    if (!q.exec()) {
        qDebug() << "loadApplicants error:" << q.lastError().text();
        return;
    }

    struct Applicant {
        QString aid, sid, status, message, name, program, sem, cvUrl;
        int realScore;
    };
    QVector<Applicant> applicants;

    MatchingEngine engine;

    while (q.next()) {
        Applicant a;
        a.aid     = q.value(0).toString();
        a.sid     = q.value(1).toString();
        a.status  = q.value(2).toString();
        a.message = q.value(4).toString();
        a.name    = q.value(5).toString();
        a.program = q.value(6).toString();
        a.sem     = q.value(7).toString();
        a.cvUrl   = q.value(8).toString();

        a.realScore = engine.calculateScore(a.sid.toInt(), pid, database);
        engine.updateEngineScore(a.sid.toInt(), pid, a.realScore * 10, database);

        applicants.append(a);
    }

    std::sort(applicants.begin(), applicants.end(),
        [](const Applicant &a, const Applicant &b){
            return a.realScore > b.realScore;
        });

    for (int row = 0; row < applicants.size(); row++) {
        const Applicant &a = applicants[row];
        applicantsTable->insertRow(row);

        applicantsTable->setItem(row, 0, new QTableWidgetItem(a.name));
        applicantsTable->setItem(row, 1, new QTableWidgetItem(a.program));
        applicantsTable->setItem(row, 2, new QTableWidgetItem(a.sem));

        auto *scoreItem = new QTableWidgetItem(
            QString::number(a.realScore) + "/10");
        scoreItem->setTextAlignment(Qt::AlignCenter);
        if (a.realScore >= 7)      scoreItem->setForeground(QColor("#16a34a"));
        else if (a.realScore >= 4) scoreItem->setForeground(QColor("#d97706"));
        else                       scoreItem->setForeground(QColor("#dc2626"));
        applicantsTable->setItem(row, 3, scoreItem);

        applicantsTable->setItem(row, 4, new QTableWidgetItem(a.message));

        if (!a.cvUrl.isEmpty() && a.cvUrl != "0") {
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
            QString url = a.cvUrl;
            connect(cvBtn, &QPushButton::clicked, this, [url]{
                QDesktopServices::openUrl(QUrl(url));
            });
            applicantsTable->setCellWidget(row, 5, cvBtn);
        } else {
            applicantsTable->setItem(row, 5, new QTableWidgetItem("No CV"));
        }

        if (a.status.toLower() == "reviewing") {
            auto *actionWidget = new QWidget();
            auto *actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(4, 2, 4, 2);
            actionLayout->setSpacing(4);

            auto *approveBtn = new QPushButton("Approve");
            approveBtn->setStyleSheet(TeacherStyles::successBtnStyle());
            approveBtn->setCursor(Qt::PointingHandCursor);

            auto *rejectBtn = new QPushButton("Reject");
            rejectBtn->setStyleSheet(TeacherStyles::dangerBtnStyle());
            rejectBtn->setCursor(Qt::PointingHandCursor);

            QString aid = a.aid;
            QString pidStr = QString::number(pid);
            connect(approveBtn, &QPushButton::clicked, this,
                [this, aid, pidStr, row]{
                    approveApplicant(aid, currentVacantSpot, pidStr, row);
                });
            connect(rejectBtn, &QPushButton::clicked, this,
                [this, aid, row]{
                    rejectApplicant(aid, row);
                });

            actionLayout->addWidget(approveBtn);
            actionLayout->addWidget(rejectBtn);
            applicantsTable->setCellWidget(row, 6, actionWidget);
        } else {
            auto *statusItem = new QTableWidgetItem(a.status.toUpper());
            statusItem->setTextAlignment(Qt::AlignCenter);
            if (a.status.toLower() == "approved")
                statusItem->setForeground(QColor("#16a34a"));
            else
                statusItem->setForeground(QColor("#dc2626"));
            applicantsTable->setItem(row, 6, statusItem);
        }
    }

    if (applicants.isEmpty()) {
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

    db.prepareAndExecute(
        "UPDATE Applications SET Status = 'APPROVED' WHERE AID = ?", {aid});

    db.prepareAndExecute(
        "UPDATE ProjectDetails SET vacantSpot = vacantSpot - 1 WHERE PID = ?",
        {pid.toInt()});

    Notification n;
    n.createAlert(aid, db.database(), true);

    auto *statusItem = new QTableWidgetItem("APPROVED");
    statusItem->setTextAlignment(Qt::AlignCenter);
    statusItem->setForeground(QColor("#16a34a"));
    applicantsTable->setItem(tableRow, 6, statusItem);

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

    db.prepareAndExecute(
        "UPDATE Applications SET Status = 'REJECTED' WHERE AID = ?", {aid});

    Notification n;
    n.createAlert(aid, db.database(), false);

    auto *statusItem = new QTableWidgetItem("REJECTED");
    statusItem->setTextAlignment(Qt::AlignCenter);
    statusItem->setForeground(QColor("#dc2626"));
    applicantsTable->setItem(tableRow, 6, statusItem);

    QMessageBox::information(this, "Rejected",
                             "Applicant rejected. A notification has been sent to the student.");
}
