#pragma once
// ─────────────────────────────────────────
//  StudentDashboard.h
//  Full sidebar + 6 content screens:
//  Skills, Interests, Browse Projects,
//  Upload CV, Recommendations, Inbox
// ─────────────────────────────────────────

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
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

class StudentDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit StudentDashboard(QWidget *parent = nullptr);
    ~StudentDashboard();

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

private:
    QStackedWidget *stack;

    // Sidebar buttons
    QPushButton *btnSkills, *btnInterests, *btnBrowse,
                *btnCV, *btnRecommend, *btnInbox;

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

    // Helpers
    QWidget* buildSkillsScreen();
    QWidget* buildInterestsScreen();
    QWidget* buildBrowseScreen();
    QWidget* buildCVScreen();
    QWidget* buildRecommendScreen();
    QWidget* buildInboxScreen();
    QWidget* makePlaceholder(const QString &text);

    QPushButton* sidebarBtn(const QString &text);
    void activateBtn(QPushButton *btn);
    QPushButton *activeBtn = nullptr;

    static QString sidebarStyle();
    static QString sidebarBtnStyle();
    static QString sidebarBtnActiveStyle();
    static QString tableStyle();
    static QString inputStyle();
    static QString primaryBtnStyle();
    static QString dangerBtnStyle();
};