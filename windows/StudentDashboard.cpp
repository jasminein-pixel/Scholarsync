#include "StudentDashboard.h"
#include <QLabel>
#include <QVBoxLayout>

StudentDashboard::StudentDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ScholarSync - Student Dashboard");
    resize(900, 600);

    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("Student Dashboard - Coming Soon");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

StudentDashboard::~StudentDashboard() {}