#ifndef TEACHERDASHBOARD_H
#define TEACHERDASHBOARD_H

#include <QWidget>

class TeacherDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherDashboard(QWidget *parent = nullptr);
    ~TeacherDashboard();
};

#endif // TEACHERDASHBOARD_H