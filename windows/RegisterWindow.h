#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

class RegisterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

signals:
    void backRequested();
    void registrationSuccessful();

private slots:
    void onRegisterClicked();
    void onBackClicked();

private:
    // Role selector
    QComboBox *roleBox;

    // Shared fields
    QLineEdit *nameInput;
    QLineEdit *emailInput;
    QLineEdit *passwordInput;
    QLineEdit *confirmPasswordInput;
    QLineEdit *contactInput;
    QLineEdit *departmentInput;

    // Student only fields
    QLineEdit *programInput;
    QComboBox *levelBox;
    QSpinBox  *semesterBox;
    QComboBox *preferenceBox;
    QWidget   *studentFieldsWidget;

    // Teacher only fields
    QLineEdit *qualificationInput;
    QWidget   *teacherFieldsWidget;

    QLabel    *statusLabel;

    void setupUI();
    void toggleRoleFields(const QString &role);
};

#endif