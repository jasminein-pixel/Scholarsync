#pragma once
// ─────────────────────────────────────────
//  StudentDashboard.h
//  Full sidebar + 6 content screens + Profile screen
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
#include <QVector>
#include <QPair>

class PieChartWidget;

class StudentDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit StudentDashboard(QWidget *parent = nullptr);
    ~StudentDashboard();

protected:
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
    QPushButton *btnSkills, *btnInterests, *btnBrowse,
        *btnCV, *btnRecommend, *btnInbox;
    void adjustTableHeight(QTableWidget *table, int dataRowCount);
    void adjustListHeight(QListWidget *list, int dataRowCount, int rowHeight);

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

    QLineEdit *profileSemesterEdit;
    QLineEdit *profileContactEdit;
    QPushButton *profileEditSaveBtn;
    bool profileEditMode = false;

    QPushButton *profileCvStatusBtn;
    QString profileCvUrl;

    QTableWidget *profileApplicationsTable;
    QTableWidget *profileCurrentTable;
    QTableWidget *profilePastTable;
    QTableWidget *profileSkillsTable;
    QTableWidget *profileInterestsTable;

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
    

// ── Recommendation caching ──
QVector<QPair<int,int>> cachedRecommendations;
bool recommendationsLoaded = false;
};
