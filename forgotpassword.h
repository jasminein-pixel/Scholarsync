#ifndef FORGOTPASSWORD_H
#define FORGOTPASSWORD_H

#include <QWidget>
#include <QStackedWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ForgotPasswordPage : public QWidget
{
    Q_OBJECT

public:
    explicit ForgotPasswordPage(QWidget *parent = nullptr);
    ~ForgotPasswordPage();
    void clearFields();

signals:
    void backRequested();
    void passwordResetSuccess();

private slots:
    void onBackClicked();
    void onNextClicked();
    void onVerifyClicked();
    void onResetClicked();

private:

    QStackedWidget *stepStack;

    // Email entry
    QWidget     *step0Widget;
    QComboBox   *roleBox;
    QLineEdit   *emailInput;
    QPushButton *nextBtn;
    QPushButton *backBtn;
    QLabel      *statusLabel0;

    // Security questions
    QWidget     *step1Widget;
    QLabel      *question1Label;
    QLabel      *question2Label;
    QLabel      *question3Label;
    QLineEdit   *answer1Input;
    QLineEdit   *answer2Input;
    QLineEdit   *answer3Input;
    QPushButton *verifyBtn;
    QLabel      *statusLabel1;

    // New password
    QWidget     *step2Widget;
    QLineEdit   *newPasswordInput;
    QLineEdit   *confirmPasswordInput;
    QPushButton *resetBtn;
    QLabel      *statusLabel2;

    QString m_email;
    QString m_role;

    static QString inputStyle();
    static QString primaryBtnStyle();
    static QString backBtnStyle();
    static QString statusStyle();
    static QString labelStyle();
    static QString questionLabelStyle();

    void buildStep0();
    void buildStep1();
    void buildStep2();
};

#endif 