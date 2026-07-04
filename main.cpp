#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Global app-wide stylesheet (light backgrounds, blue accents)
    app.setStyleSheet(R"(
        QWidget {
            font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
        }
        QLineEdit {
            border: 1px solid #d0d7e6;
            border-radius: 8px;
            padding: 10px 12px;
            font-size: 13px;
            background: #ffffff;
        }
        QLineEdit:focus {
            border: 1px solid #2f54eb;
        }
        QPushButton#primaryButton {
            background-color: #1d3fd6;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton#primaryButton:hover {
            background-color: #1733b0;
        }
        QPushButton#secondaryButton {
            background-color: transparent;
            color: #1d3fd6;
            border: 1px solid #1d3fd6;
            border-radius: 8px;
            padding: 12px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton#secondaryButton:hover {
            background-color: #eef1ff;
        }
        QPushButton#linkButton {
            background: transparent;
            border: none;
            color: #1d3fd6;
            font-size: 12px;
            text-align: right;
        }
        QPushButton#backButton {
            background: transparent;
            border: none;
            color: #1d3fd6;
            font-weight: 600;
            font-size: 13px;
            text-align: left;
        }
        QPushButton#roleCard {
            background-color: #f5f7fc;
            border: 1px solid #e3e8f5;
            border-radius: 12px;
            padding: 16px;
            text-align: left;
        }
        QPushButton#roleCard:hover {
            background-color: #eef1ff;
            border: 1px solid #c7d2f5;
        }
    )");

    MainWindow window;
    window.resize(420, 720);
    window.setWindowTitle("ScholarSync - Login");
    window.show();

    return app.exec();
}