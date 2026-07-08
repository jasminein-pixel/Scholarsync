#include "StudentDashboard.h"
#include "../database/DatabaseManager.h"
#include "../fileupload.h"
#include "../notification.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QPen>
#include <QIntValidator>

// ── Pull in session globals from mainwindow.h ──
extern int currentSID;
extern QString currentUserName;

// ═══════════════════════════════════════════════
//  Small custom pie chart widget (no QtCharts dependency)
// ═══════════════════════════════════════════════
class PieChartWidget : public QWidget
{
public:
    struct Slice {
        QString label;
        int value;
        QColor color;
    };

    explicit PieChartWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumHeight(230);
        setMinimumWidth(380);
    }

    void setData(const QVector<Slice> &data)
    {
        slices = data;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        int total = 0;
        for (const auto &s : slices) total += s.value;

        const int legendWidth = 190;
        const int margin = 18;

        int availableW = width() - legendWidth - margin * 2;
        int availableH = height() - margin * 2;
        int side = qMin(availableW, availableH);
        side = qMax(side, 60);

        QRect pieRect(margin, (height() - side) / 2, side, side);

        if (total == 0) {
            p.setPen(QColor("#94a3b8"));
            p.drawText(rect(), Qt::AlignCenter, "No applications submitted yet.");
            return;
        }

        // ── Donut slices ──
        double startAngle = 90.0 * 16;
        for (const auto &s : slices) {
            if (s.value <= 0) continue;
            double span = -1.0 * s.value / total * 360.0 * 16;
            p.setBrush(s.color);
            p.setPen(QPen(Qt::white, 3));
            p.drawPie(pieRect, static_cast<int>(startAngle), static_cast<int>(span));
            startAngle += span;
        }

        // ── Donut hole + center total ──
        int holeMargin = static_cast<int>(side * 0.32);
        QRect holeRect = pieRect.adjusted(holeMargin, holeMargin, -holeMargin, -holeMargin);
        p.setBrush(QColor("white"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(holeRect);

        p.setPen(QColor("#0f172a"));
        QFont bigFont(font().family(), 15, QFont::Bold);
        p.setFont(bigFont);
        p.drawText(holeRect, Qt::AlignCenter, QString::number(total));

        // ── Legend, docked to the right and vertically centered ──
        const int rowHeight = 26;
        int legendX = pieRect.right() + 34;
        int legendBlockHeight = rowHeight * slices.size();
        int legendY = qMax(margin, (height() - legendBlockHeight) / 2);

        QFont legendFont(font().family(), 9);
        p.setFont(legendFont);
        for (const auto &s : slices) {
            p.setBrush(s.color);
            p.setPen(Qt::NoPen);
            p.drawEllipse(legendX, legendY + 3, 11, 11);

            p.setPen(QColor("#334155"));
            QString pct = QString::number(s.value * 100.0 / total, 'f', 0) + "%";
            p.drawText(legendX + 20, legendY + 12,
                       QString("%1 — %2 (%3)").arg(s.label).arg(s.value).arg(pct));
            legendY += rowHeight;
        }
    }

private:
    QVector<Slice> slices;
};

// ═══════════════════════════════════════════════
//  Style helpers
// ═══════════════════════════════════════════════
QString StudentDashboard::sidebarStyle() {
    return "background-color: #1e3a8a; color: white;";
}
QString StudentDashboard::sidebarBtnStyle() {
    return R"(
        QPushButton {
            color: #bfcfef;
            background-color: transparent;
            border: none;
            text-align: left;
            padding: 12px 20px;
            font-size: 13px;
            border-radius: 0;
        }
        QPushButton:hover {
            background-color: #2d4fa3;
            color: white;
        }
    )";
}
QString StudentDashboard::sidebarBtnActiveStyle() {
    return R"(
        QPushButton {
            color: white;
            background-color: #2d4fa3;
            border-left: 3px solid #60a5fa;
            text-align: left;
            padding: 12px 20px;
            font-size: 13px;
            font-weight: 600;
        }
    )";
}
QString StudentDashboard::tableStyle() {
    return R"(
        QTableWidget {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            gridline-color: #f1f5f9;
            font-size: 13px;
            color: #1e293b;
        }
        QTableWidget::item { padding: 8px; }
        QTableWidget::item:selected {
            background: #eff6ff;
            color: #1e3a8a;
        }
        QHeaderView::section {
            background: #f8fafc;
            color: #64748b;
            font-weight: 600;
            font-size: 12px;
            padding: 10px;
            border: none;
            border-bottom: 1px solid #e2e8f0;
        }
    )";
}
QString StudentDashboard::inputStyle() {
    return R"(
        QLineEdit, QSpinBox, QComboBox {
            border: 1.5px solid #cbd5e1;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            background: white;
            color: #1e293b;
            min-height: 36px;
        }
        QLineEdit:focus, QSpinBox:focus {
            border-color: #3b82f6;
        }
    )";
}
QString StudentDashboard::primaryBtnStyle() {
    return R"(
        QPushButton {
            background-color: #1d4ed8;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 9px 18px;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #1e40af; }
    )";
}
QString StudentDashboard::dangerBtnStyle() {
    return R"(
        QPushButton {
            background-color: #ef4444;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 9px 18px;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #dc2626; }
    )";
}

// ═══════════════════════════════════════════════
//  Sidebar button helpers
// ═══════════════════════════════════════════════
QPushButton* StudentDashboard::sidebarBtn(const QString &text) {
    auto *btn = new QPushButton(text);
    btn->setStyleSheet(sidebarBtnStyle());
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);
    return btn;
}

void StudentDashboard::activateBtn(QPushButton *btn) {
    if (activeBtn) activeBtn->setStyleSheet(sidebarBtnStyle());
    activeBtn = btn;
    btn->setStyleSheet(sidebarBtnActiveStyle());
}

// ═══════════════════════════════════════════════
//  Constructor
// ═══════════════════════════════════════════════
StudentDashboard::StudentDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Student Dashboard");
    resize(1000, 650);
    setStyleSheet("background-color: #f8fafc;");

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ──
    auto *sidebar = new QWidget();
    sidebar->setFixedWidth(210);
    sidebar->setStyleSheet(sidebarStyle());
    auto *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Logo area
    auto *logoArea = new QWidget();
    logoArea->setStyleSheet("background-color: #172554; padding: 4px;");
    auto *logoLayout = new QVBoxLayout(logoArea);
    auto *logoLabel = new QLabel("ScholarSync");
    logoLabel->setStyleSheet(
        "color: white; font-size: 16px; font-weight: 700; padding: 16px 20px 4px;");

    // Username label — now a member so it can be clicked to open the profile page
    nameLabel = new QLabel(currentUserName.isEmpty() ? "Student" : currentUserName);
    nameLabel->setStyleSheet(
        "color: #93c5fd; font-size: 12px; padding: 0 20px 16px; text-decoration: underline;");
    nameLabel->setCursor(Qt::PointingHandCursor);
    nameLabel->setToolTip("View profile");
    nameLabel->installEventFilter(this);

    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(nameLabel);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->addWidget(logoArea);

    // Nav label
    auto *navLabel = new QLabel("NAVIGATION");
    navLabel->setStyleSheet(
        "color: #64748b; font-size: 10px; font-weight: 700; "
        "padding: 16px 20px 8px; letter-spacing: 1px;");
    sideLayout->addWidget(navLabel);

    // Nav buttons
    btnSkills    = sidebarBtn("My Skills");
    btnInterests = sidebarBtn("My Interests");
    btnBrowse    = sidebarBtn("Browse Projects");
    btnCV        = sidebarBtn("Upload CV");
    btnRecommend = sidebarBtn("Recommendations");
    btnInbox     = sidebarBtn("Inbox");

    for (auto *b : {btnSkills, btnInterests, btnBrowse, btnCV, btnRecommend, btnInbox})
        sideLayout->addWidget(b);

    sideLayout->addStretch();

    // ── Content stack ──
    stack = new QStackedWidget();
    stack->addWidget(buildSkillsScreen());       // 0
    stack->addWidget(buildInterestsScreen());    // 1
    stack->addWidget(buildBrowseScreen());       // 2
    stack->addWidget(buildCVScreen());           // 3
    stack->addWidget(buildRecommendScreen());    // 4
    stack->addWidget(buildInboxScreen());        // 5
    stack->addWidget(buildProfileScreen());      // 6 (PROFILE_SCREEN_INDEX)

    root->addWidget(sidebar);
    root->addWidget(stack);

    // ── Wire sidebar buttons ──
    connect(btnSkills, &QPushButton::clicked, this, [this]{
        activateBtn(btnSkills);
        stack->setCurrentIndex(0);
        loadSkills();
    });
    connect(btnInterests, &QPushButton::clicked, this, [this]{
        activateBtn(btnInterests);
        stack->setCurrentIndex(1);
        loadInterests();
    });
    connect(btnBrowse, &QPushButton::clicked, this, [this]{
        activateBtn(btnBrowse);
        stack->setCurrentIndex(2);
        loadProjects();
    });
    connect(btnCV, &QPushButton::clicked, this, [this]{
        activateBtn(btnCV);
        stack->setCurrentIndex(3);
    });
    connect(btnRecommend, &QPushButton::clicked, this, [this]{
        activateBtn(btnRecommend);
        stack->setCurrentIndex(4);
        loadRecommendations();
    });
    connect(btnInbox, &QPushButton::clicked, this, [this]{
        activateBtn(btnInbox);
        stack->setCurrentIndex(5);
        loadInbox();
    });

    // Default screen
    activateBtn(btnSkills);
    loadSkills();
}

StudentDashboard::~StudentDashboard() {}

// ═══════════════════════════════════════════════
//  Event filter — detects clicks on the username label
// ═══════════════════════════════════════════════
bool StudentDashboard::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == nameLabel && event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            openProfile();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

// ═══════════════════════════════════════════════
//  Opens the profile screen
// ═══════════════════════════════════════════════
void StudentDashboard::openProfile()
{
    // No sidebar button stays highlighted while on the profile screen
    if (activeBtn) activeBtn->setStyleSheet(sidebarBtnStyle());
    activeBtn = nullptr;

    stack->setCurrentIndex(PROFILE_SCREEN_INDEX);
    loadProfile();
}

// ═══════════════════════════════════════════════
//  Screen builders
// ═══════════════════════════════════════════════

// ── Helper: screen title ──
static QLabel* screenTitle(const QString &text) {
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 20px; font-weight: 700; color: #0f172a; "
        "background: transparent; margin-bottom: 4px;");
    return lbl;
}

static QLabel* screenSub(const QString &text) {
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 13px; color: #64748b; background: transparent; margin-bottom: 16px;");
    return lbl;
}

// Small helper for the profile info card: bold label + value pair (read-only)
static QWidget* profileInfoField(const QString &caption, QLabel *&valueLabelOut) {
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

// Small helper for the profile info card: bold label + value pair that can be
// switched into an editable QLineEdit (used for Semester & Contact Info).
static QWidget* profileInfoFieldEditable(const QString &caption,
                                         QLabel *&valueLabelOut,
                                         QLineEdit *&editOut) {
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

// Small helper: section header inside the profile screen
static QLabel* profileSectionHeader(const QString &text) {
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet(
        "font-size: 14px; font-weight: 700; color: #0f172a; "
        "background: transparent; margin-top: 12px; margin-bottom: 4px;");
    return lbl;
}

// Turns a DB status value (any casing, e.g. "ACCEPTED", "reviewing", "PENDING")
// into a clean, grammatically-proper label for display.
static QString prettyStatus(const QString &raw) {
    QString s = raw.trimmed().toLower();
    if (s.isEmpty()) return "Unknown";
    if (s == "pending") s = "reviewing"; // legacy/default value maps to Reviewing
    s[0] = s[0].toUpper();
    return s;
}

// ══════════════
//  SKILLS SCREEN
// ══════════════
QWidget* StudentDashboard::buildSkillsScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("My Skills"));
    layout->addWidget(screenSub("Manage your technical skills and proficiency levels."));

    // Table
    skillsTable = new QTableWidget(0, 3);
    skillsTable->setHorizontalHeaderLabels({"Skill", "Proficiency (1-10)", "Action"});
    skillsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    skillsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    skillsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    skillsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    skillsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    skillsTable->verticalHeader()->hide();
    skillsTable->setStyleSheet(tableStyle());
    skillsTable->setMinimumHeight(300);
    layout->addWidget(skillsTable);

    // Add skill row
    auto *addRow = new QHBoxLayout();
    skillNameInput = new QLineEdit();
    skillNameInput->setPlaceholderText("Skill name (e.g. Python)");
    skillNameInput->setStyleSheet(inputStyle());
    skillProfInput = new QSpinBox();
    skillProfInput->setRange(1, 10);
    skillProfInput->setValue(5);
    skillProfInput->setStyleSheet(inputStyle());
    skillProfInput->setFixedWidth(80);

    auto *addBtn = new QPushButton("Add Skill");
    addBtn->setStyleSheet(primaryBtnStyle());
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

// ══════════════════
//  INTERESTS SCREEN
// ══════════════════
QWidget* StudentDashboard::buildInterestsScreen()
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
    )");
    interestsList->setMinimumHeight(300);
    layout->addWidget(interestsList);

    auto *addRow = new QHBoxLayout();
    interestInput = new QLineEdit();
    interestInput->setPlaceholderText("e.g. Machine Learning, Robotics");
    interestInput->setStyleSheet(inputStyle());

    auto *addBtn = new QPushButton("Add Interest");
    addBtn->setStyleSheet(primaryBtnStyle());
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &StudentDashboard::addInterest);

    auto *delBtn = new QPushButton("Remove Selected");
    delBtn->setStyleSheet(dangerBtnStyle());
    delBtn->setCursor(Qt::PointingHandCursor);
    connect(delBtn, &QPushButton::clicked, this, &StudentDashboard::deleteInterest);

    addRow->addWidget(interestInput);
    addRow->addWidget(addBtn);
    addRow->addWidget(delBtn);
    layout->addLayout(addRow);
    layout->addStretch();

    return w;
}

// ═════════════════════
//  BROWSE PROJECTS SCREEN
// ═════════════════════
QWidget* StudentDashboard::buildBrowseScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Browse Projects"));
    layout->addWidget(screenSub("Explore active research projects available for application."));

    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("🔄  Refresh");
    refreshBtn->setStyleSheet(primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &StudentDashboard::loadProjects);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    projectsTable = new QTableWidget(0, 6);
    projectsTable->setHorizontalHeaderLabels({
                                              "Project Name", "Department", "Duration", "Spots", "Preference", "Action"});
    projectsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    projectsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    projectsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    projectsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    projectsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    projectsTable->verticalHeader()->hide();
    projectsTable->setStyleSheet(tableStyle());
    layout->addWidget(projectsTable);

    return w;
}

// ═══════════════
//  CV SCREEN
// ═══════════════
QWidget* StudentDashboard::buildCVScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(16);

    layout->addWidget(screenTitle("Upload CV"));
    layout->addWidget(screenSub("Upload your CV/Resume so professors can review your profile."));

    // Card
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
    uploadBtn->setStyleSheet(primaryBtnStyle());
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

// ══════════════════════
//  RECOMMENDATIONS SCREEN
// ══════════════════════
QWidget* StudentDashboard::buildRecommendScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Recommendations"));
    layout->addWidget(screenSub(
        "Projects recommended based on your skills and interests, ranked by match score."));

    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("Refresh");
    refreshBtn->setStyleSheet(primaryBtnStyle());
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked,
            this, &StudentDashboard::loadRecommendations);
    topRow->addStretch();
    topRow->addWidget(refreshBtn);
    layout->addLayout(topRow);

    recommendTable = new QTableWidget(0, 4);
    recommendTable->setHorizontalHeaderLabels({
                                               "Project Name", "Department", "Match Score", "Action"});
    recommendTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    recommendTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    recommendTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    recommendTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    recommendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recommendTable->verticalHeader()->hide();
    recommendTable->setStyleSheet(tableStyle());
    layout->addWidget(recommendTable);

    return w;
}

// ═══════════════
//  INBOX SCREEN
// ═══════════════
QWidget* StudentDashboard::buildInboxScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");
    auto *layout = new QVBoxLayout(w);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    layout->addWidget(screenTitle("Inbox"));
    layout->addWidget(screenSub("Notifications about your applications and credits."));

    auto *topRow = new QHBoxLayout();
    auto *refreshBtn = new QPushButton("🔄  Refresh");
    refreshBtn->setStyleSheet(primaryBtnStyle());
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
    )");
    layout->addWidget(inboxList);

    return w;
}

// ═══════════════
//  PROFILE SCREEN
// ═══════════════
QWidget* StudentDashboard::buildProfileScreen()
{
    auto *w = new QWidget();
    w->setStyleSheet("background: #f8fafc;");

    auto *outer = new QVBoxLayout(w);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ── Scroll area so the page works even with lots of rows ──
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

    // ── Info card (no border — just a soft white panel) ──
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

    // Name row + Edit/Save button
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

    // Fields grid (2 rows x 4 columns)
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

    // Restrict Semester edit box to sensible integer values
    profileSemesterEdit->setValidator(new QIntValidator(1, 20, profileSemesterEdit));
    profileContactEdit->setPlaceholderText("Phone number");

    // CV status shortcut
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
    connect(profileCvStatusBtn, &QPushButton::clicked, this, [this]{
        if (!profileCvUrl.isEmpty() && profileCvUrl != "0") {
            QDesktopServices::openUrl(QUrl(profileCvUrl));
        } else {
            stack->setCurrentIndex(3);
            activateBtn(btnCV);
        }
    });
    infoGrid->addWidget(profileCvStatusBtn);

    layout->addWidget(infoCard);

    // ── My Skills (read-only view — edit from the "My Skills" sidebar page) ──
    layout->addWidget(profileSectionHeader("My Skills"));
    profileSkillsTable = new QTableWidget(0, 2);
    profileSkillsTable->setHorizontalHeaderLabels({"Skill", "Proficiency"});
    profileSkillsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profileSkillsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profileSkillsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileSkillsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profileSkillsTable->verticalHeader()->hide();
    profileSkillsTable->setStyleSheet(tableStyle());
    profileSkillsTable->setMinimumHeight(140);
    layout->addWidget(profileSkillsTable);

    // Edit / Save toggle logic
    connect(profileEditSaveBtn, &QPushButton::clicked, this, [this]{
        if (!profileEditMode) {
            // Enter edit mode
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
            // Save changes back to the database
            auto &db = DatabaseManager::instance();
            QString semesterVal = profileSemesterEdit->text().trimmed();
            QString contactVal  = profileContactEdit->text().trimmed();

            db.prepareAndExecute(
                "UPDATE StudentDetails SET Semester = ?, ContactInfo = ? WHERE SID = ?",
                {semesterVal, contactVal, currentSID});

            profileEditSaveBtn->setText("Edit Details");
            profileEditMode = false;
            loadProfile(); // refreshes labels and switches back to display mode
        }
    });

    // ── Applications table (lifetime) ──
    layout->addWidget(profileSectionHeader("Applications"));
    profileApplicationsTable = new QTableWidget(0, 4);
    profileApplicationsTable->setHorizontalHeaderLabels({
                                                         "Project Name", "Department", "Status", "Score"});
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    profileApplicationsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    profileApplicationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileApplicationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profileApplicationsTable->verticalHeader()->hide();
    profileApplicationsTable->setStyleSheet(tableStyle());
    profileApplicationsTable->setMinimumHeight(160);
    layout->addWidget(profileApplicationsTable);

    // ── Current Projects table (now includes Supervisor) ──
    layout->addWidget(profileSectionHeader("Current Projects"));
    profileCurrentTable = new QTableWidget(0, 4);
    profileCurrentTable->setHorizontalHeaderLabels({
                                                    "Project Name", "Department", "Duration", "Supervisor"});
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    profileCurrentTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    profileCurrentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileCurrentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profileCurrentTable->verticalHeader()->hide();
    profileCurrentTable->setStyleSheet(tableStyle());
    profileCurrentTable->setMinimumHeight(140);
    layout->addWidget(profileCurrentTable);

    // ── Past Projects table ──
    layout->addWidget(profileSectionHeader("Past Projects"));
    profilePastTable = new QTableWidget(0, 2);
    profilePastTable->setHorizontalHeaderLabels({"Project Name", "Department"});
    profilePastTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    profilePastTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    profilePastTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profilePastTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profilePastTable->verticalHeader()->hide();
    profilePastTable->setStyleSheet(tableStyle());
    profilePastTable->setMinimumHeight(140);
    layout->addWidget(profilePastTable);

    // ── Lifetime application status breakdown (pie chart) ──
    layout->addWidget(profileSectionHeader("Application History (Lifetime)"));
    profileAppsChart = new PieChartWidget();
    profileAppsChart->setStyleSheet("background: white; border-radius: 8px;");
    layout->addWidget(profileAppsChart);

    layout->addStretch();

    scrollArea->setWidget(content);
    outer->addWidget(scrollArea);

    return w;
}

// ═══════════════════════════════════════════════
//  Data loading functions
// ═══════════════════════════════════════════════

void StudentDashboard::loadSkills()
{
    skillsTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.prepareAndExecute(
        "SELECT SkillName, Proficiency FROM SkillList WHERE SID = ?",
        {currentSID});

    int row = 0;
    while (q.next()) {
        skillsTable->insertRow(row);
        skillsTable->setItem(row, 0,
                             new QTableWidgetItem(q.value(0).toString()));
        skillsTable->setItem(row, 1,
                             new QTableWidgetItem(q.value(1).toString()));

        // Delete button
        auto *delBtn = new QPushButton("Remove");
        delBtn->setStyleSheet(dangerBtnStyle());
        delBtn->setCursor(Qt::PointingHandCursor);
        QString skillName = q.value(0).toString();
        connect(delBtn, &QPushButton::clicked, this, [this, skillName]{
            auto &db = DatabaseManager::instance();
            db.prepareAndExecute(
                "DELETE FROM SkillList WHERE SID = ? AND SkillName = ?",
                {currentSID, skillName});
            loadSkills();
        });
        skillsTable->setCellWidget(row, 2, delBtn);
        row++;
    }

    if (row == 0) {
        skillsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No skills added yet. Add one below!");
        empty->setForeground(QColor("#94a3b8"));
        skillsTable->setItem(0, 0, empty);
    }
}

void StudentDashboard::addSkill()
{
    QString name = skillNameInput->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Missing", "Please enter a skill name.");
        return;
    }
    int prof = skillProfInput->value();

    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    // Check duplicate
    auto check = db.prepareAndExecute(
        "SELECT COUNT(*) FROM SkillList WHERE SID = ? AND SkillName = ?",
        {currentSID, name});
    check.next();
    if (check.value(0).toInt() > 0) {
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
    if (row < 0) {
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

void StudentDashboard::loadInterests()
{
    interestsList->clear();
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.prepareAndExecute(
        "SELECT Preference FROM StudentDetails WHERE SID = ?",
        {currentSID});

    if (q.next()) {
        QString prefs = q.value(0).toString();
        if (!prefs.isEmpty()) {
            QStringList items = prefs.split(",", Qt::SkipEmptyParts);
            for (auto &item : items)
                interestsList->addItem(item.trimmed());
        }
    }
}

void StudentDashboard::addInterest()
{
    QString interest = interestInput->text().trimmed();
    if (interest.isEmpty()) return;

    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    // Get current preferences
    auto q = db.prepareAndExecute(
        "SELECT Preference FROM StudentDetails WHERE SID = ?", {currentSID});
    QString current = "";
    if (q.next()) current = q.value(0).toString();

    QString updated = current.isEmpty() ? interest : current + ", " + interest;

    db.prepareAndExecute(
        "UPDATE StudentDetails SET Preference = ? WHERE SID = ?",
        {updated, currentSID});

    interestInput->clear();
    loadInterests();
}

void StudentDashboard::deleteInterest()
{
    QListWidgetItem *item = interestsList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Select", "Please select an interest to remove.");
        return;
    }
    QString toRemove = item->text();

    auto &db = DatabaseManager::instance();
    auto q = db.prepareAndExecute(
        "SELECT Preference FROM StudentDetails WHERE SID = ?", {currentSID});
    if (q.next()) {
        QString current = q.value(0).toString();
        QStringList items = current.split(",", Qt::SkipEmptyParts);
        items.removeIf([&](const QString &s){
            return s.trimmed().toLower() == toRemove.toLower();
        });
        QString updated = items.join(", ");
        db.prepareAndExecute(
            "UPDATE StudentDetails SET Preference = ? WHERE SID = ?",
            {updated, currentSID});
    }
    loadInterests();
}

void StudentDashboard::loadProjects()
{
    projectsTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.executeQuery(
        "SELECT PID, ProjectName, department, expectedDuration, vacantSpot, preference "
        "FROM ProjectDetails WHERE status = 'Active' AND vacantSpot > 0");

    int row = 0;
    while (q.next()) {
        projectsTable->insertRow(row);
        int pid = q.value(0).toInt();
        projectsTable->setItem(row, 0, new QTableWidgetItem(q.value(1).toString()));
        projectsTable->setItem(row, 1, new QTableWidgetItem(q.value(2).toString()));
        projectsTable->setItem(row, 2, new QTableWidgetItem(q.value(3).toString()));
        projectsTable->setItem(row, 3, new QTableWidgetItem(q.value(4).toString()));
        projectsTable->setItem(row, 4, new QTableWidgetItem(q.value(5).toString()));

        // Apply button
        auto *applyBtn = new QPushButton("Apply");
        applyBtn->setStyleSheet(primaryBtnStyle());
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this, pid]{
            auto &db = DatabaseManager::instance();

            // Check if already applied
            auto check = db.prepareAndExecute(
                "SELECT COUNT(*) FROM Applications WHERE SID = ? AND PID = ?",
                {currentSID, pid});
            check.next();
            if (check.value(0).toInt() > 0) {
                QMessageBox::information(this, "Already Applied",
                                         "You have already applied to this project.");
                return;
            }

            db.prepareAndExecute(
                "INSERT INTO Applications (SID, PID, Status, EngineScore, message) "
                "VALUES (?, ?, 'reviewing', 0, 'Application submitted via ScholarSync')",
                {currentSID, pid});

            QMessageBox::information(this, "Applied!",
                                     "Your application has been submitted successfully.");
            loadProjects();
        });
        projectsTable->setCellWidget(row, 5, applyBtn);
        row++;
    }

    if (row == 0) {
        projectsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No active projects available right now.");
        empty->setForeground(QColor("#94a3b8"));
        projectsTable->setItem(0, 0, empty);
    }
}

void StudentDashboard::uploadCV()
{
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    QSqlDatabase database = db.database();  // store in local variable first

    File f;
    bool result = f.uploadCV(
        QString::number(currentSID),
        currentUserName,
        database    // pass the local variable, not a temporary
        );

    if (result) {
        cvStatusLabel->setStyleSheet(
            "font-size: 13px; color: #16a34a; border: none; background: transparent;");
        cvStatusLabel->setText("CV uploaded successfully!");
    } else {
        cvStatusLabel->setStyleSheet(
            "font-size: 13px; color: #dc2626; border: none; background: transparent;");
        cvStatusLabel->setText("Upload cancelled or failed.");
    }
}

void StudentDashboard::loadRecommendations()
{
    recommendTable->setRowCount(0);
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    // Get student skills
    auto skillQ = db.prepareAndExecute(
        "SELECT SkillName FROM SkillList WHERE SID = ?", {currentSID});
    QStringList studentSkills;
    while (skillQ.next())
        studentSkills << skillQ.value(0).toString().toLower();

    // Get all active projects
    auto projQ = db.executeQuery(
        "SELECT PID, ProjectName, department FROM ProjectDetails "
        "WHERE status = 'Active' AND vacantSpot > 0");

    QVector<QPair<int, QString>> scored; // {score, row data}
    QVector<QStringList> rows;

    while (projQ.next()) {
        int pid = projQ.value(0).toInt();
        QString name = projQ.value(1).toString();
        QString dept = projQ.value(2).toString();

        // Get required skills for this project
        auto reqQ = db.prepareAndExecute(
            "SELECT skillName FROM skillRequirement WHERE PID = ?", {pid});
        int matches = 0, total = 0;
        while (reqQ.next()) {
            total++;
            QString req = reqQ.value(0).toString().toLower();
            if (studentSkills.contains(req)) matches++;
        }

        int score = total > 0 ? (matches * 100 / total) : 0;
        scored.append({score, name});
        rows.append({name, dept, QString::number(score) + "%", QString::number(pid)});
    }

    // Sort by score descending
    QVector<int> indices(rows.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](int a, int b){
        return scored[a].first > scored[b].first;
    });

    for (int i : indices) {
        int row = recommendTable->rowCount();
        recommendTable->insertRow(row);
        recommendTable->setItem(row, 0, new QTableWidgetItem(rows[i][0]));
        recommendTable->setItem(row, 1, new QTableWidgetItem(rows[i][1]));

        // Score badge
        auto *scoreItem = new QTableWidgetItem(rows[i][2]);
        scoreItem->setTextAlignment(Qt::AlignCenter);
        int score = scored[i].first;
        if (score >= 70)
            scoreItem->setForeground(QColor("#16a34a"));
        else if (score >= 40)
            scoreItem->setForeground(QColor("#d97706"));
        else
            scoreItem->setForeground(QColor("#dc2626"));
        recommendTable->setItem(row, 2, scoreItem);

        // Apply button
        int pid = rows[i][3].toInt();
        auto *applyBtn = new QPushButton("Apply");
        applyBtn->setStyleSheet(primaryBtnStyle());
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this, pid]{
            auto &db = DatabaseManager::instance();
            auto check = db.prepareAndExecute(
                "SELECT COUNT(*) FROM Applications WHERE SID = ? AND PID = ?",
                {currentSID, pid});
            check.next();
            if (check.value(0).toInt() > 0) {
                QMessageBox::information(this, "Already Applied",
                                         "You have already applied to this project.");
                return;
            }
            db.prepareAndExecute(
                "INSERT INTO Applications (SID, PID, Status, EngineScore, message) "
                "VALUES (?, ?, 'reviewing', 0, 'Application submitted via ScholarSync')",
                {currentSID, pid});
            QMessageBox::information(this, "Applied!",
                                     "Your application has been submitted.");
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

void StudentDashboard::loadInbox()
{
    inboxList->clear();
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    auto q = db.prepareAndExecute(
        "SELECT Info, Type, ReceivedAt FROM StudentInbox "
        "WHERE SID = ? ORDER BY ReceivedAt DESC",
        {currentSID});

    while (q.next()) {
        QString info = q.value(0).toString();
        QString type = q.value(1).toString();
        QString time = q.value(2).toString();

        QString icon = type == "CREDIT ALERT" ? "⭐ " : "📬 ";
        QString display = icon + info + "\n" + time;

        auto *item = new QListWidgetItem(display);
        if (type == "CREDIT ALERT")
            item->setForeground(QColor("#d97706"));
        inboxList->addItem(item);
    }

    if (inboxList->count() == 0) {
        inboxList->addItem("Your inbox is empty.");
    }
}

// ═══════════════════════════════════════════════
//  Profile data loading
//
//  NOTE ON SCHEMA:
//    StudentDetails: Email, Semester, Program, Department, Level,
//      Preference, Credit, ContactInfo, cv_url ('0' means no CV on file).
//    Applications.Status only ever holds: Reviewing (or legacy 'Pending'),
//      Accepted, Rejected — read case-insensitively via LOWER().
//    ProjectDetails.status holds the project's own lifecycle: Active or
//      Completed — also read case-insensitively.
//    Combined meaning used throughout this screen:
//      accepted + active project    -> Current/Active project
//      accepted + completed project -> Past project
//      reviewing / rejected         -> only ever shown in the Applications list
// ═══════════════════════════════════════════════
void StudentDashboard::loadProfile()
{
    auto &db = DatabaseManager::instance();
    if (!db.isConnected()) db.connect();

    // Always reset the info card to display (non-edit) mode when the
    // profile is opened or refreshed.
    profileSemesterLabel->show();
    profileSemesterEdit->hide();
    profileContactLabel->show();
    profileContactEdit->hide();
    profileEditMode = false;
    profileEditSaveBtn->setText("Edit Details");

    profileNameLabel->setText(currentUserName.isEmpty() ? "Student" : currentUserName);

    auto textOr = [](const QVariant &v) -> QString {
        QString s = v.toString().trimmed();
        return s.isEmpty() ? QStringLiteral("—") : s;
    };

    // ── Basic info ──
    auto infoQ = db.prepareAndExecute(
        "SELECT Email, Semester, Program, Department, Level, Preference, Credit, ContactInfo, cv_url "
        "FROM StudentDetails WHERE SID = ?",
        {currentSID});

    if (infoQ.next()) {
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
    } else {
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

    // ── My Skills (read-only) ──
    profileSkillsTable->setRowCount(0);
    auto skillsQ = db.prepareAndExecute(
        "SELECT SkillName, Proficiency FROM SkillList WHERE SID = ? ORDER BY Proficiency DESC",
        {currentSID});

    int row = 0;
    while (skillsQ.next()) {
        profileSkillsTable->insertRow(row);
        profileSkillsTable->setItem(row, 0,
                                    new QTableWidgetItem(skillsQ.value(0).toString()));
        profileSkillsTable->setItem(row, 1,
                                    new QTableWidgetItem(skillsQ.value(1).toString() + " / 10"));
        row++;
    }
    if (row == 0) {
        profileSkillsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No skills added yet.");
        empty->setForeground(QColor("#94a3b8"));
        profileSkillsTable->setItem(0, 0, empty);
    }

    // ── Applications (all, lifetime) ──
    profileApplicationsTable->setRowCount(0);
    auto appQ = db.prepareAndExecute(
        "SELECT p.ProjectName, p.department, a.Status, a.EngineScore "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "WHERE a.SID = ? "
        "ORDER BY a.AppliedAt DESC",
        {currentSID});

    row = 0;
    while (appQ.next()) {
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
            statusItem->setForeground(QColor("#d97706")); // reviewing / pending
        profileApplicationsTable->setItem(row, 2, statusItem);

        profileApplicationsTable->setItem(row, 3,
                                          new QTableWidgetItem(appQ.value(3).toString()));
        row++;
    }
    if (row == 0) {
        profileApplicationsTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No applications submitted yet.");
        empty->setForeground(QColor("#94a3b8"));
        profileApplicationsTable->setItem(0, 0, empty);
    }

    // ── Current projects: my application was accepted AND the project is
    //    still active (Application status: accepted; Project status: active) ──
    profileCurrentTable->setRowCount(0);
    auto curQ = db.prepareAndExecute(
        "SELECT p.ProjectName, p.department, p.expectedDuration, t.Name "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "LEFT JOIN TeacherDetails t ON p.TID = t.TID "
        "WHERE a.SID = ? AND LOWER(a.Status) = 'accepted' AND LOWER(p.status) = 'active'",
        {currentSID});

    row = 0;
    while (curQ.next()) {
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
    if (row == 0) {
        profileCurrentTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No current projects.");
        empty->setForeground(QColor("#94a3b8"));
        profileCurrentTable->setItem(0, 0, empty);
    }

    // ── Past projects: only ones where my application was accepted AND
    //    the project itself has since been marked completed ──
    profilePastTable->setRowCount(0);
    auto pastQ = db.prepareAndExecute(
        "SELECT p.ProjectName, p.department "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "WHERE a.SID = ? AND LOWER(a.Status) = 'accepted' AND LOWER(p.status) = 'completed'",
        {currentSID});

    row = 0;
    while (pastQ.next()) {
        profilePastTable->insertRow(row);
        profilePastTable->setItem(row, 0,
                                  new QTableWidgetItem(pastQ.value(0).toString()));
        profilePastTable->setItem(row, 1,
                                  new QTableWidgetItem(pastQ.value(1).toString()));
        row++;
    }
    if (row == 0) {
        profilePastTable->insertRow(0);
        auto *empty = new QTableWidgetItem("No past projects.");
        empty->setForeground(QColor("#94a3b8"));
        profilePastTable->setItem(0, 0, empty);
    }

    // ── Lifetime application breakdown (pie chart) ──
    //    Reviewing / Rejected come straight from the application status.
    //    Accepted applications are split by the project's own status:
    //    accepted + active project    -> "Active"
    //    accepted + completed project -> "Completed"
    auto histQ = db.prepareAndExecute(
        "SELECT LOWER(a.Status), LOWER(p.status), COUNT(*) "
        "FROM Applications a "
        "JOIN ProjectDetails p ON a.PID = p.PID "
        "WHERE a.SID = ? "
        "GROUP BY LOWER(a.Status), LOWER(p.status)",
        {currentSID});

    int cReviewing = 0, cRejected = 0, cActive = 0, cCompleted = 0;
    while (histQ.next()) {
        QString appStatus  = histQ.value(0).toString().trimmed().toLower();
        QString projStatus = histQ.value(1).toString().trimmed().toLower();
        int cnt = histQ.value(2).toInt();

        if (appStatus == "reviewing" || appStatus == "pending") {
            cReviewing += cnt;
        } else if (appStatus == "rejected") {
            cRejected += cnt;
        } else if (appStatus == "accepted") {
            if (projStatus == "completed") cCompleted += cnt;
            else cActive += cnt; // active, or any other project state defaults to Active
        }
    }

    profileAppsChart->setData({
        {"Reviewing", cReviewing, QColor("#93c5fd")}, // light blue — matches sidebar accent
        {"Active",    cActive,    QColor("#3b82f6")}, // mid blue
        {"Completed", cCompleted, QColor("#1e3a8a")}, // deep navy — matches sidebar bg
        {"Rejected",  cRejected,  QColor("#ef4444")}  // red accent for the one negative outcome
    });
}

QWidget* StudentDashboard::makePlaceholder(const QString &text) {
    auto *w = new QWidget();
    auto *l = new QVBoxLayout(w);
    auto *lbl = new QLabel(text);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("font-size: 16px; color: #94a3b8;");
    l->addWidget(lbl);
    return w;
}