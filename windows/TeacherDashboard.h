#pragma once
// ─────────────────────────────────────────
//  TeacherDashboard.h
//  Sidebar + 3 screens:
//  My Projects, Post Project, Applicants
// ─────────────────────────────────────────

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class TeacherDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherDashboard(QWidget *parent = nullptr);
    ~TeacherDashboard();

private slots:
    // My Projects
    void loadMyProjects();

    // Post Project
    void postProject();
    void addSkillRequirement();
    void removeSkillRequirement();

    // Applicants
    void loadApplicantsForProject(int pid, const QString &projectName);
    void approveApplicant(const QString &aid, const QString &vacantSpot,
                          const QString &pid, int tableRow);
    void rejectApplicant(const QString &aid, int tableRow);

private:
    QStackedWidget *stack;

    // Sidebar buttons
    QPushButton *btnProjects, *btnPost, *btnApplicants;
    QPushButton *activeBtn = nullptr;

    // ── My Projects screen ──
    QTableWidget *projectsTable;

    // ── Post Project screen ──
    QLineEdit   *projNameInput;
    QTextEdit   *projDescInput;
    QSpinBox    *projSpotsInput;
    QLineEdit   *projDurationInput;
    QLineEdit   *projDeptInput;
    QComboBox   *projPrefInput;
    QListWidget *skillReqList;
    QLineEdit   *skillReqNameInput;
    QSpinBox    *skillReqProfInput;
    QLabel      *postStatusLabel;

    // ── Applicants screen ──
    QTableWidget *applicantsTable;
    QLabel       *applicantsTitle;
    int           currentPID = -1;
    QString       currentVacantSpot;

    // Screen builders
    QWidget* buildProjectsScreen();
    QWidget* buildPostScreen();
    QWidget* buildApplicantsScreen();

    // Style helpers
    QPushButton* sidebarBtn(const QString &text);
    void activateBtn(QPushButton *btn);

    static QString sidebarStyle();
    static QString sidebarBtnStyle();
    static QString sidebarBtnActiveStyle();
    static QString tableStyle();
    static QString inputStyle();
    static QString primaryBtnStyle();
    static QString successBtnStyle();
    static QString dangerBtnStyle();
};