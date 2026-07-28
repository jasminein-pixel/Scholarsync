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

QWidget *StudentDashboard::buildInterestsScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("My Interests"));
    layout->addWidget(screenSub("Add research areas and topics you're passionate about."));

    interestsList = new QListWidget();
    interestsList->setStyleSheet(R"(
        QListWidget {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            font-size: 13px;
            color: #1e293b;
            padding: 4px;
        }
        QListWidget::item {
            padding: 10px 12px;
            border-radius: 4px;
        }
        QListWidget::item:selected {
            background: #eff6ff;
            color: #1e3a8a;
        }
        QListWidget::item:hover { background: #f8fafc; }
        QScrollBar:vertical {
            background: transparent;
            width: 10px;
            margin: 2px;
        }
        QScrollBar::handle:vertical {
            background: #cbd5e1;
            border-radius: 5px;
            min-height: 24px;
        }
        QScrollBar::handle:vertical:hover { background: #94a3b8; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
    )");
    interestsList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    interestsList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(interestsList);

    auto *addRow = new QHBoxLayout();
    interestInput = new QLineEdit();
    interestInput->setPlaceholderText("e.g. Machine Learning, Robotics");
    interestInput->setStyleSheet(StudentStyles::inputStyle());

    auto *addBtn = new QPushButton("Add Interest");
    addBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &StudentDashboard::addInterest);

    auto *delBtn = new QPushButton("Remove Selected");
    delBtn->setStyleSheet(StudentStyles::dangerBtnStyle());
    delBtn->setCursor(Qt::PointingHandCursor);
    connect(delBtn, &QPushButton::clicked, this, &StudentDashboard::deleteInterest);

    addRow->addWidget(interestInput);
    addRow->addWidget(addBtn);
    addRow->addWidget(delBtn);
    layout->addLayout(addRow);
    layout->addStretch();

    return w;
}

void StudentDashboard::loadInterests()
{
    interestsList->setUpdatesEnabled(false);
    interestsList->clear();
    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    auto q = db.prepareAndExecute(
        "SELECT Interest FROM Interest WHERE SID = ? ORDER BY Interest ASC",
        {currentSID});

    int count = 0;
    while (q.next())
    {
        interestsList->addItem(q.value(0).toString());
        count++;
    }

    if (count == 0)
    {
        auto *placeholder = new QListWidgetItem("No interests added yet.");
        placeholder->setFlags(placeholder->flags() & ~Qt::ItemIsSelectable);
        placeholder->setForeground(QColor("#94a3b8"));
        interestsList->addItem(placeholder);
    }

    adjustListHeight(interestsList, count, 34);
    interestsList->setUpdatesEnabled(true);
}

void StudentDashboard::addInterest()
{
    QString interest = interestInput->text().trimmed();
    if (interest.isEmpty())
        return;

    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    auto check = db.prepareAndExecute(
        "SELECT COUNT(*) FROM Interest WHERE SID = ? AND Interest = ?",
        {currentSID, interest});
    check.next();
    if (check.value(0).toInt() > 0)
    {
        QMessageBox::warning(this, "Duplicate", "You already have this interest.");
        interestInput->clear();
        return;
    }

    db.prepareAndExecute(
        "INSERT INTO Interest (SID, Interest) VALUES (?, ?)",
        {currentSID, interest});

    interestInput->clear();
    loadInterests();
}

void StudentDashboard::deleteInterest()
{
    QListWidgetItem *item = interestsList->currentItem();
    if (!item || !(item->flags() & Qt::ItemIsSelectable))
    {
        QMessageBox::warning(this, "Select", "Please select an interest to remove.");
        return;
    }
    QString toRemove = item->text();

    auto &db = DatabaseManager::instance();
    db.prepareAndExecute(
        "DELETE FROM Interest WHERE SID = ? AND Interest = ?",
        {currentSID, toRemove});
    loadInterests();
}
