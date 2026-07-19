#ifndef TEACHER_LOGIN_H
#define TEACHER_LOGIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class TeacherLogin;
}
QT_END_NAMESPACE

class TeacherLogin : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherLogin(QWidget *parent = nullptr);
    ~TeacherLogin();

    void clearFields();

signals:
    void backRequested();
    void loginAttempted(const QString &email, const QString &password);
    void registerRequested();
    void forgotPasswordRequested();

private slots:
    void onBackClicked();
    void onLoginClicked();
    void onForgotPasswordClicked();
    void onRegisterClicked();
    void onTogglePasswordVisibility();

private:
    Ui::TeacherLogin *ui;
};

#endif // TEACHER_LOGIN_H