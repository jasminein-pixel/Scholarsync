#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Global app-wide stylesheet (light backgrounds, blue accents)
    app.setStyleSheet(R"(
    QWidget {
        font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
        background-color: #f0f4ff;
        color: #16213e;
    }
    QMainWindow {
        background-color: #f0f4ff;
    }
    QLabel {
        color: #16213e;
        background-color: transparent;
    }
    QLineEdit {
        border: 1.5px solid #c0c8e0;
        border-radius: 8px;
        padding: 10px 12px;
        font-size: 13px;
        background: #ffffff;
        color: #16213e;
    }
    QLineEdit:focus {
        border: 1.5px solid #2f54eb;
    }
    QLineEdit::placeholder {
        color: #9aa3b2;
    }
    QComboBox {
        border: 1.5px solid #c0c8e0;
        border-radius: 8px;
        padding: 8px 12px;
        font-size: 13px;
        background: #ffffff;
        color: #16213e;
    }
    QComboBox:focus {
        border: 1.5px solid #2f54eb;
    }
    QComboBox::drop-down {
        border: none;
    }
    QSpinBox {
        border: 1.5px solid #c0c8e0;
        border-radius: 8px;
        padding: 8px 12px;
        font-size: 13px;
        background: #ffffff;
        color: #16213e;
    }
    QPushButton#primaryButton {
        background-color: #1d3fd6;
        color: #ffffff;
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
        border: 1.5px solid #1d3fd6;
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
        background-color: #1E3A8A;
        color: #ffffff;
        border: 1px solid #e3e8f5;
        border-radius: 12px;
        padding: 16px;
        text-align: left;
        font-size: 13px;
    }
    QPushButton#roleCard:hover {
        background-color: #1a3275;
        border: 1px solid #c7d2f5;
    }
    QScrollArea {
        background-color: #f0f4ff;
        border: none;
    }
    QScrollBar:vertical {
        background: #f0f4ff;
        width: 8px;
    }
    QScrollBar::handle:vertical {
        background: #c0c8e0;
        border-radius: 4px;
    }
)");

    MainWindow window;
    window.resize(420, 720);
    window.setWindowTitle("ScholarSync - Login");
    window.show();

    return app.exec();
}