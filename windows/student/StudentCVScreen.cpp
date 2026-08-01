#include "StudentDashboard.h"
#include "StudentStyles.h"
#include "../../database/DatabaseManager.h"
#include "../../services/fileupload.h"

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

QWidget *StudentDashboard::buildCVScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(16);

    layout->addWidget(screenTitle("Upload CV"));
    layout->addWidget(screenSub("Upload your CV/Resume so professors can review your profile."));

    auto *card = new QWidget();
    card->setStyleSheet(R"(
        QWidget {
            background: white;
            border: 2px dashed #cbd5e1;
            border-radius: 12px;
        }
    )");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(16);
    cardLayout->setAlignment(Qt::AlignCenter);

    auto *icon = new QLabel("📄");
    icon->setStyleSheet("font-size: 48px; border: none; background: transparent;");
    icon->setAlignment(Qt::AlignCenter);

    auto *cardTitle = new QLabel("Select your CV file");
    cardTitle->setStyleSheet(
        "font-size: 16px; font-weight: 600; color: #0f172a; "
        "border: none; background: transparent;");
    cardTitle->setAlignment(Qt::AlignCenter);

    auto *cardSub = new QLabel("Supported formats: PDF, DOCX");
    cardSub->setStyleSheet(
        "font-size: 13px; color: #64748b; border: none; background: transparent;");
    cardSub->setAlignment(Qt::AlignCenter);

    auto *uploadBtn = new QPushButton("Choose File & Upload");
    uploadBtn->setStyleSheet(StudentStyles::primaryBtnStyle());
    uploadBtn->setMinimumHeight(44);
    uploadBtn->setCursor(Qt::PointingHandCursor);
    connect(uploadBtn, &QPushButton::clicked, this, &StudentDashboard::uploadCV);

    cvStatusLabel = new QLabel("");
    cvStatusLabel->setAlignment(Qt::AlignCenter);
    cvStatusLabel->setStyleSheet(
        "font-size: 13px; color: #16a34a; border: none; background: transparent;");
    cvStatusLabel->setWordWrap(true);

    cardLayout->addWidget(icon);
    cardLayout->addWidget(cardTitle);
    cardLayout->addWidget(cardSub);
    cardLayout->addWidget(uploadBtn);
    cardLayout->addWidget(cvStatusLabel);

    layout->addWidget(card);
    layout->addStretch();

    return w;
}

void StudentDashboard::uploadCV()
{
    auto &db = DatabaseManager::instance();
    if (!db.isConnected())
        db.connect();

    QSqlDatabase database = db.database();

    File f;
    bool result = f.uploadCV(
        QString::number(currentSID),
        currentUserName,
        database
    );

    if (result)
    {
        cvStatusLabel->setStyleSheet(
            "font-size: 13px; color: #16a34a; border: none; background: transparent;");
        cvStatusLabel->setText("CV uploaded successfully!");
    }
    else
    {
        cvStatusLabel->setStyleSheet(
            "font-size: 13px; color: #dc2626; border: none; background: transparent;");
        cvStatusLabel->setText("Upload cancelled or failed.");
    }
}
