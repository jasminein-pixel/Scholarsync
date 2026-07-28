#include "StudentDashboard.h"
#include "StudentStyles.h"
#include "PieChartWidget.h"
#include "../../database/DatabaseManager.h"
#include "../../views/student_login.h"
#include "../../mainwindow.h"

// ── Session globals from mainwindow.h ──
extern int currentSID;
extern QString currentUserName;

QPushButton *StudentDashboard::sidebarBtn(const QString &text)
{
    auto *btn = new QPushButton(text);
    btn->setStyleSheet(StudentStyles::sidebarBtnStyle());
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);
    return btn;
}

void StudentDashboard::activateBtn(QPushButton *btn)
{
    if (activeBtn)
        activeBtn->setStyleSheet(StudentStyles::sidebarBtnStyle());
    activeBtn = btn;
    btn->setStyleSheet(StudentStyles::sidebarBtnActiveStyle());
}

void StudentDashboard::adjustTableHeight(QTableWidget *table, int dataRowCount)
{
    const int rowHeight = table->verticalHeader()->defaultSectionSize();
    const int headerHeight = table->horizontalHeader()->height();
    const int frame = table->frameWidth() * 2 + 2;

    int visibleRows = qBound(1, dataRowCount, 5);
    int height = headerHeight + rowHeight * visibleRows + frame;

    table->setMinimumHeight(height);
    table->setMaximumHeight(height);
}

void StudentDashboard::adjustListHeight(QListWidget *list, int dataRowCount, int rowHeight)
{
    const int frame = list->frameWidth() * 2 + 8;

    int visibleRows = qBound(1, dataRowCount, 5);
    int height = rowHeight * visibleRows + frame;

    list->setMinimumHeight(height);
    list->setMaximumHeight(height);
}

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
    sidebar->setStyleSheet(StudentStyles::sidebarStyle());
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

    auto *navLabel = new QLabel("NAVIGATION");
    navLabel->setStyleSheet(
        "color: #64748b; font-size: 10px; font-weight: 700; "
        "padding: 16px 20px 8px; letter-spacing: 1px;");
    sideLayout->addWidget(navLabel);

    btnSkills = sidebarBtn("My Skills");
    btnInterests = sidebarBtn("My Interests");
    btnBrowse = sidebarBtn("Browse Projects");
    btnCV = sidebarBtn("Upload CV");
    btnRecommend = sidebarBtn("Recommendations");
    btnInbox = sidebarBtn("Inbox");

    for (auto *b : {btnSkills, btnInterests, btnBrowse, btnCV, btnRecommend, btnInbox})
        sideLayout->addWidget(b);

    sideLayout->addStretch();

    auto *btnLogout = new QPushButton("Logout");
    btnLogout->setStyleSheet(StudentStyles::dangerBtnStyle());
    btnLogout->setCursor(Qt::PointingHandCursor);
    btnLogout->setMinimumHeight(44);

    sideLayout->addWidget(btnLogout);

    connect(btnLogout, &QPushButton::clicked, this, [this]()
    {
        currentSID = -1;
        currentUserName.clear();

        MainWindow *mainWin = new MainWindow();
        mainWin->setAttribute(Qt::WA_DeleteOnClose);
        mainWin->goToStudentLogin();
        mainWin->show();

        close();
    });

    // ── Content stack ──
    stack = new QStackedWidget();
    stack->addWidget(buildSkillsScreen());    // 0
    stack->addWidget(buildInterestsScreen()); // 1
    stack->addWidget(buildBrowseScreen());    // 2
    stack->addWidget(buildCVScreen());        // 3
    stack->addWidget(buildRecommendScreen()); // 4
    stack->addWidget(buildInboxScreen());     // 5
    stack->addWidget(buildProfileScreen());   // 6

    root->addWidget(sidebar);
    root->addWidget(stack);

    connect(btnSkills, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnSkills);
        stack->setCurrentIndex(0);
        loadSkills();
    });
    connect(btnInterests, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnInterests);
        stack->setCurrentIndex(1);
        loadInterests();
    });
    connect(btnBrowse, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnBrowse);
        stack->setCurrentIndex(2);
        loadProjects();
    });
    connect(btnCV, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnCV);
        stack->setCurrentIndex(3);
    });
    connect(btnRecommend, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnRecommend);
        stack->setCurrentIndex(4);
        loadRecommendations();
    });
    connect(btnInbox, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnInbox);
        stack->setCurrentIndex(5);
        loadInbox();
    });

    activateBtn(btnSkills);
    loadSkills();
}

StudentDashboard::~StudentDashboard() {}

bool StudentDashboard::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == nameLabel && event->type() == QEvent::MouseButtonPress)
    {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            openProfile();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void StudentDashboard::openProfile()
{
    if (activeBtn)
        activeBtn->setStyleSheet(StudentStyles::sidebarBtnStyle());
    activeBtn = nullptr;

    stack->setCurrentIndex(PROFILE_SCREEN_INDEX);
    loadProfile();
}

QWidget *StudentDashboard::makePlaceholder(const QString &text)
{
    auto *w = new QWidget();
    auto *l = new QVBoxLayout(w);
    auto *lbl = new QLabel(text);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("font-size: 16px; color: #94a3b8;");
    l->addWidget(lbl);
    return w;
}
