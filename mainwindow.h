#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class StudentLogin;
class TeacherLogin;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Role selection page
    void onStudentRoleSelected();
    void onTeacherRoleSelected();

    // Navigation back to role selection (from either login page)
    void onBackToRoleSelect();

    // Hooked up to StudentLogin / TeacherLogin login attempts.
    // Replace the bodies of these with real Azure SQL / nanodbc auth calls.
    void handleStudentLoginAttempt(const QString &email, const QString &password);
    void handleTeacherLoginAttempt(const QString &email, const QString &password);

private:
    enum StackIndex {
        PageRoleSelect = 0,
        PageStudentLogin = 1,
        PageTeacherLogin = 2
    };

    Ui::MainWindow *ui;
    StudentLogin *m_studentLoginPage;
    TeacherLogin *m_teacherLoginPage;

    void setupConnections();
};
#endif // MAINWINDOW_H