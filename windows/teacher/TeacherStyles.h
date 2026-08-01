#pragma once
#include <QString>

namespace TeacherStyles
{
    inline QString sidebarStyle()
    {
        return "background-color: #1e3a8a; color: white;";
    }

    inline QString sidebarBtnStyle()
    {
        return R"(
            QPushButton {
                color: #bfcfef;
                background-color: transparent;
                border: none;
                text-align: left;
                padding: 12px 20px;
                font-size: 13px;
                border-radius: 0;
            }
            QPushButton:hover {
                background-color: #2d4fa3;
                color: white;
            }
        )";
    }

    inline QString sidebarBtnActiveStyle()
    {
        return R"(
            QPushButton {
                color: white;
                background-color: #2d4fa3;
                border-left: 3px solid #60a5fa;
                text-align: left;
                padding: 12px 20px;
                font-size: 13px;
                font-weight: 600;
            }
        )";
    }

    inline QString tableStyle()
    {
        return R"(
            QTableWidget {
                background: white;
                border: 1px solid #e2e8f0;
                border-radius: 8px;
                gridline-color: #f1f5f9;
                font-size: 13px;
                color: #1e293b;
            }
            QTableWidget::item { padding: 8px; }
            QTableWidget::item:selected {
                background: #eff6ff;
                color: #1e3a8a;
            }
            QHeaderView::section {
                background: #f8fafc;
                color: #64748b;
                font-weight: 600;
                font-size: 12px;
                padding: 10px;
                border: none;
                border-bottom: 1px solid #e2e8f0;
            }
        )";
    }

    inline QString inputStyle()
    {
        return R"(
            QLineEdit, QSpinBox, QComboBox, QTextEdit {
                border: 1.5px solid #cbd5e1;
                border-radius: 6px;
                padding: 8px 12px;
                font-size: 13px;
                background: white;
                color: #1e293b;
                min-height: 36px;
            }
            QLineEdit:focus, QSpinBox:focus, QTextEdit:focus {
                border-color: #3b82f6;
            }
        )";
    }

    inline QString primaryBtnStyle()
    {
        return R"(
            QPushButton {
                background-color: #1d4ed8;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 9px 18px;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton:hover { background-color: #1e40af; }
        )";
    }

    inline QString successBtnStyle()
    {
        return R"(
            QPushButton {
                background-color: #16a34a;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 7px 14px;
                font-size: 12px;
                font-weight: 600;
            }
            QPushButton:hover { background-color: #15803d; }
        )";
    }

    inline QString dangerBtnStyle()
    {
        return R"(
            QPushButton {
                background-color: #ef4444;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 7px 14px;
                font-size: 12px;
                font-weight: 600;
            }
            QPushButton:hover { background-color: #dc2626; }
        )";
    }
}
