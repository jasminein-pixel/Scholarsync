#include "StudentDashboard.h"
#include "StudentStyles.h"
#include "../../database/DatabaseManager.h"
#include "../../services/modification.h"

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

QWidget *StudentDashboard::buildInboxScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Inbox"));
    layout->addWidget(screenSub("Notifications about your applications and credits."));

    auto *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(0);
    auto *refreshBtn = new QPushButton("Refresh");
    refreshBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
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
        QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
        QScrollBar::handle:vertical { background: #cbd5e1; border-radius: 5px; min-height: 24px; }
        QScrollBar::handle:vertical:hover { background: #94a3b8; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
    )");
    inboxList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    inboxList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(inboxList);

    return w;
}

void StudentDashboard::loadInbox()
{
    inboxList->clear();
    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    auto q = db.prepareAndExecute(
        "SELECT Info, Type, ReceivedAt FROM StudentInbox "
        "WHERE SID = ? ORDER BY ReceivedAt DESC",
        {currentSID});

    while (q.next())
    {
        QString info = q.value(0).toString();
        QString type = q.value(1).toString();
        QString rawTime = q.value(2).toString();

        QString date, time;
        formatTime(rawTime, date, time);

        QString icon = type == "CREDIT ALERT" ? "✪" : "● ";
        QString display = icon + info + "\n" + date + " " + time;

        auto *item = new QListWidgetItem(display);
        if (type == "CREDIT ALERT")
            item->setForeground(QColor("#d97706"));
        inboxList->addItem(item);
    }

    if (inboxList->count() == 0)
    {
        inboxList->addItem("Your inbox is empty.");
    }
}
