#ifndef STUDENT_LOGIN_H
#define STUDENT_LOGIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class StudentLogin;
}
QT_END_NAMESPACE

class StudentLogin : public QWidget
{
    Q_OBJECT

public:
    explicit StudentLogin(QWidget *parent = nullptr);
    ~StudentLogin();

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
    Ui::StudentLogin *ui;
};

#endif // STUDENT_LOGIN_H
