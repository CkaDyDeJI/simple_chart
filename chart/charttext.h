#ifndef CHARTTEXT_H
#define CHARTTEXT_H

#include "chartlayeritem.h"

class PlainChart;

class ChartText : public ChartLayerItem
{
public:
    explicit ChartText(PlainChart* chart = NULL);

    virtual void paint(QPainter* painter);

    void addText(const QVector<QPointF>& points, const QVector<QString>& strs);
    void addText(const QPointF& point, const QString& str);
    void addAbsText(const QVector<QPointF>& points, const QVector<QString>& strs);
    void addAbsText(const QPointF& point, const QString& str);
    void clearData() { place.clear(); data.clear(); }
    void clearAbsData() { placeAbs.clear(); dataAbs.clear(); }
    void setTextPen(QPen newPen) { textPen = newPen; }

private:
    void initPainter(QPainter* painter);
    void paintPointsText(QPainter* painter);

    PlainChart* chart;
    QVector<QPointF> place;
    QVector<QPointF> placeAbs;
    QVector<QString> data;
    QVector<QString> dataAbs;
    QPen textPen;
};

#endif // CHARTTEXT_H
