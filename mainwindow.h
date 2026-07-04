#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class StudentLogin;
class TeacherLogin;

// Global session variables
inline int currentSID = -1;
inline int currentTID = -1;
inline QString currentUserName = "";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStudentRoleSelected();
    void onTeacherRoleSelected();
    void onBackToRoleSelect();
    void handleStudentLoginAttempt(const QString &email, const QString &password);
    void handleTeacherLoginAttempt(const QString &email, const QString &password);
    void onStudentRegisterRequested();
    void onTeacherRegisterRequested();

private:
    enum StackIndex {
        PageRoleSelect   = 0,
        PageStudentLogin = 1,
        PageTeacherLogin = 2
    };

    Ui::MainWindow    *ui;
    StudentLogin      *m_studentLoginPage;
    TeacherLogin      *m_teacherLoginPage;

    void setupConnections();
};

#endif // MAINWINDOW_H