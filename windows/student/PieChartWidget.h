#pragma once
#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>

// ═══════════════════════════════════════════════
//  Custom pie / donut chart widget (no QtCharts dependency)
// ═══════════════════════════════════════════════
class PieChartWidget : public QWidget
{
    Q_OBJECT
public:
    struct Slice
    {
        QString label;
        int value;
        QColor color;
    };

    explicit PieChartWidget(QWidget *parent = nullptr);
    void setData(const QVector<Slice> &data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<Slice> slices;
};
