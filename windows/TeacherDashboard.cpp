#include "TeacherDashboard.h"
#include <QLabel>
#include <QVBoxLayout>

TeacherDashboard::TeacherDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Teacher Dashboard");
    resize(900, 600);

    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("Teacher Dashboard - Coming Soon");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

TeacherDashboard::~TeacherDashboard() {}