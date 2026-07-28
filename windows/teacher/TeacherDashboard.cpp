#include "TeacherDashboard.h"
#include "TeacherStyles.h"
#include "../../database/DatabaseManager.h"
#include "../../views/teacher_login.h"
#include "../../mainwindow.h"

extern int currentTID;
extern QString currentUserName;

QPushButton *TeacherDashboard::sidebarBtn(const QString &text)
{
    auto *btn = new QPushButton(text);
    btn->setStyleSheet(TeacherStyles::sidebarBtnStyle());
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);
    return btn;
}

void TeacherDashboard::activateBtn(QPushButton *btn)
{
    if (activeBtn)
        activeBtn->setStyleSheet(TeacherStyles::sidebarBtnStyle());
    activeBtn = btn;
    btn->setStyleSheet(TeacherStyles::sidebarBtnActiveStyle());
}

TeacherDashboard::TeacherDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Teacher Dashboard");
    resize(1100, 680);
    setStyleSheet("background-color: #f8fafc;");

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ──
    auto *sidebar = new QWidget();
    sidebar->setFixedWidth(220);
    sidebar->setStyleSheet(TeacherStyles::sidebarStyle());
    auto *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Logo area
    auto *logoArea = new QWidget();
    logoArea->setStyleSheet("background-color: #172554; padding: 4px;");
    auto *logoLayout = new QVBoxLayout(logoArea);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    auto *logoLabel = new QLabel("ScholarSync");
    logoLabel->setStyleSheet(
        "color: white; font-size: 16px; font-weight: 700; padding: 16px 20px 4px;");
    auto *nameLabel = new QLabel(
        currentUserName.isEmpty() ? "Professor" : "Prof. " + currentUserName);
    nameLabel->setStyleSheet(
        "color: #93c5fd; font-size: 12px; padding: 0 20px 16px;");
    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(nameLabel);
    sideLayout->addWidget(logoArea);

    auto *navLabel = new QLabel("NAVIGATION");
    navLabel->setStyleSheet(
        "color: #64748b; font-size: 10px; font-weight: 700; "
        "padding: 16px 20px 8px; letter-spacing: 1px;");
    sideLayout->addWidget(navLabel);

    btnProjects = sidebarBtn("My Projects");
    btnPost = sidebarBtn("Post a Project");
    btnApplicants = sidebarBtn("View Applicants");

    sideLayout->addWidget(btnProjects);
    sideLayout->addWidget(btnPost);
    sideLayout->addWidget(btnApplicants);
    sideLayout->addStretch();

    auto *btnLogout = new QPushButton("Logout");
    btnLogout->setStyleSheet(TeacherStyles::dangerBtnStyle());
    btnLogout->setCursor(Qt::PointingHandCursor);
    btnLogout->setMinimumHeight(44);

    sideLayout->addWidget(btnLogout);

    connect(btnLogout, &QPushButton::clicked, this, [this]()
    {
        currentTID = -1;
        currentUserName.clear();

        MainWindow *mainWin = new MainWindow();
        mainWin->setAttribute(Qt::WA_DeleteOnClose);
        mainWin->goToTeacherLogin();
        mainWin->show();

        close();
    });

    // ── Content stack ──
    stack = new QStackedWidget();
    stack->addWidget(buildProjectsScreen());   // 0
    stack->addWidget(buildPostScreen());       // 1
    stack->addWidget(buildApplicantsScreen()); // 2

    root->addWidget(sidebar);
    root->addWidget(stack);

    connect(btnProjects, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnProjects);
        stack->setCurrentIndex(0);
        loadMyProjects();
    });
    connect(btnPost, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnPost);
        stack->setCurrentIndex(1);
    });
    connect(btnApplicants, &QPushButton::clicked, this, [this]
    {
        activateBtn(btnApplicants);
        stack->setCurrentIndex(2);
        if (currentPID == -1)
            applicantsTitle->setText(
                "Select a project from 'My Projects' to view its applicants.");
    });

    activateBtn(btnProjects);
    loadMyProjects();
}

TeacherDashboard::~TeacherDashboard() {}
