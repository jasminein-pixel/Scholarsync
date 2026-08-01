#include "PieChartWidget.h"
#include <QPainter>
#include <QPen>
#include <QtGlobal>

PieChartWidget::PieChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(230);
    setMinimumWidth(380);
}

void PieChartWidget::setData(const QVector<Slice> &data)
{
    slices = data;
    update();
}

void PieChartWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int total = 0;
    for (const auto &s : slices)
        total += s.value;

    const int legendWidth = 190;
    const int margin = 18;

    int availableW = width() - legendWidth - margin * 2;
    int availableH = height() - margin * 2;
    int side = qMin(availableW, availableH);
    side = qMax(side, 60);

    QRect pieRect(margin, (height() - side) / 2, side, side);

    if (total == 0)
    {
        p.setPen(QColor("#94a3b8"));
        p.drawText(rect(), Qt::AlignCenter, "No applications submitted yet.");
        return;
    }

    // ── Donut slices ──
    double startAngle = 90.0 * 16;
    for (const auto &s : slices)
    {
        if (s.value <= 0)
            continue;
        double span = -1.0 * s.value / total * 360.0 * 16;
        p.setBrush(s.color);
        p.setPen(QPen(Qt::white, 3));
        p.drawPie(pieRect, static_cast<int>(startAngle), static_cast<int>(span));
        startAngle += span;
    }

    // ── Donut hole + center total ──
    int holeMargin = static_cast<int>(side * 0.32);
    QRect holeRect = pieRect.adjusted(holeMargin, holeMargin, -holeMargin, -holeMargin);
    p.setBrush(QColor("white"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(holeRect);

    p.setPen(QColor("#0f172a"));
    QFont bigFont(font().family(), 15, QFont::Bold);
    p.setFont(bigFont);
    p.drawText(holeRect, Qt::AlignCenter, QString::number(total));

    // ── Legend ──
    const int rowHeight = 26;
    int legendX = pieRect.right() + 34;
    int legendBlockHeight = rowHeight * slices.size();
    int legendY = qMax(margin, (height() - legendBlockHeight) / 2);

    QFont legendFont(font().family(), 9);
    p.setFont(legendFont);
    for (const auto &s : slices)
    {
        p.setBrush(s.color);
        p.setPen(Qt::NoPen);
        p.drawEllipse(legendX, legendY + 3, 11, 11);

        p.setPen(QColor("#334155"));
        QString pct = QString::number(s.value * 100.0 / total, 'f', 0) + "%";
        p.drawText(legendX + 20, legendY + 12,
                   QString("%1 — %2 (%3)").arg(s.label).arg(s.value).arg(pct));
        legendY += rowHeight;
    }
}
