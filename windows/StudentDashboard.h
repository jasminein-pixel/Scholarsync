#ifndef STUDENTDASHBOARD_H
#define STUDENTDASHBOARD_H

#include <QWidget>

class StudentDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit StudentDashboard(QWidget *parent = nullptr);
    ~StudentDashboard();
};

#endif // STUDENTDASHBOARD_H