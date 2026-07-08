#pragma once
// ─────────────────────────────────────────
//  StudentDashboard.h
//  Full sidebar + 6 content screens:
//  Skills, Interests, Browse Projects,
//  Upload CV, Recommendations, Inbox
//  + Profile screen (opened by clicking username)
// ─────────────────────────────────────────
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

class PieChartWidget; // defined in StudentDashboard.cpp

class StudentDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit StudentDashboard(QWidget *parent = nullptr);
    ~StudentDashboard();

protected:
    // Used to detect clicks on the username label (QLabel has no clicked() signal)
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    // Skills
    void loadSkills();
    void addSkill();
    void deleteSkill();
    // Interests
    void loadInterests();
    void addInterest();
    void deleteInterest();
    // Browse Projects
    void loadProjects();
    // Upload CV
    void uploadCV();
    // Recommendations
    void loadRecommendations();
    // Inbox
    void loadInbox();
    // Profile
    void loadProfile();

private:
    QStackedWidget *stack;
    // Sidebar buttons
    QPushButton *btnSkills, *btnInterests, *btnBrowse,
        *btnCV, *btnRecommend, *btnInbox;

    // Username label in sidebar (clickable -> opens profile)
    QLabel *nameLabel;

    static const int PROFILE_SCREEN_INDEX = 6;

    // ── Skills screen widgets ──
    QTableWidget *skillsTable;
    QLineEdit    *skillNameInput;
    QSpinBox     *skillProfInput;
    // ── Interests screen widgets ──
    QListWidget *interestsList;
    QLineEdit   *interestInput;
    // ── Browse Projects screen ──
    QTableWidget *projectsTable;
    // ── CV screen ──
    QLabel *cvStatusLabel;
    // ── Recommendations screen ──
    QTableWidget *recommendTable;
    // ── Inbox screen ──
    QListWidget *inboxList;

    // ── Profile screen widgets ──
    QLabel *profileNameLabel;
    QLabel *profileEmailLabel;
    QLabel *profileSemesterLabel;
    QLabel *profileProgramLabel;
    QLabel *profileCreditsLabel;
    QLabel *profileDepartmentLabel;
    QLabel *profileLevelLabel;
    QLabel *profilePreferenceLabel;
    QLabel *profileContactLabel;

    // Editable fields (Semester + Contact Info)
    QLineEdit *profileSemesterEdit;
    QLineEdit *profileContactEdit;
    QPushButton *profileEditSaveBtn;
    bool profileEditMode = false;

    // CV status shortcut (click -> opens the CV if one exists, else Upload CV screen)
    QPushButton *profileCvStatusBtn;
    QString profileCvUrl;

    QTableWidget *profileApplicationsTable;  // all applications / applied projects (lifetime)
    QTableWidget *profileCurrentTable;       // currently active projects
    QTableWidget *profilePastTable;          // completed / past projects
    QTableWidget *profileSkillsTable;        // read-only view of My Skills

    // Lifetime application status breakdown
    PieChartWidget *profileAppsChart;

    // Helpers
    QWidget* buildSkillsScreen();
    QWidget* buildInterestsScreen();
    QWidget* buildBrowseScreen();
    QWidget* buildCVScreen();
    QWidget* buildRecommendScreen();
    QWidget* buildInboxScreen();
    QWidget* buildProfileScreen();
    QWidget* makePlaceholder(const QString &text);
    QPushButton* sidebarBtn(const QString &text);
    void activateBtn(QPushButton *btn);
    void openProfile();
    QPushButton *activeBtn = nullptr;

    static QString sidebarStyle();
    static QString sidebarBtnStyle();
    static QString sidebarBtnActiveStyle();
    static QString tableStyle();
    static QString inputStyle();
    static QString primaryBtnStyle();
    static QString dangerBtnStyle();
};