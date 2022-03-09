#include "charttext.h"
#include "chartdata.h"
#include "chartaxis.h"
#include "plainchart.h"

#include <QPainter>

ChartText::ChartText(PlainChart* chart)
    : ChartLayerItem(),
      chart(chart)
{
    textPen = QPen(Qt::gray, 5, Qt::SolidLine);
}

void ChartText::paint(QPainter* painter)
{
    const QTransform oldTr = painter->transform();
    const QRect oldWindow = painter->window();

    initPainter(painter);

    paintPointsText(painter);

    painter->setTransform(oldTr);
    painter->setWindow(oldWindow);
}

void ChartText::addText(const QVector<QPointF>& points, const QVector<QString>& strs)
{
    Q_ASSERT(points.size() == strs.size());

    for (int i = 0; i < points.size(); ++i)
        addText(points.at(i), strs.at(i));
}

void ChartText::addText(const QPointF& point, const QString& str)
{
    place.append(point);
    data.append(str);
}

void ChartText::addAbsText(const QVector<QPointF>& points, const QVector<QString>& strs)
{
    Q_ASSERT(points.size() == strs.size());

    for (int i = 0; i < points.size(); ++i)
        addAbsText(points.at(i), strs.at(i));
}

void ChartText::addAbsText(const QPointF& point, const QString& str)
{
    placeAbs.append(point);
    dataAbs.append(str);
}

void ChartText::initPainter(QPainter* painter)
{
    painter->resetTransform();
    painter->setPen(textPen);
}

void ChartText::paintPointsText(QPainter* painter)
{
    //отрисовка абсолютных точек (в пикселях)
    for (int i = 0; i < placeAbs.size(); ++i)
        painter->drawText(placeAbs.at(i), dataAbs.at(i));

    //отрисовка подписей к точкам (в координатах)
    QVector<QPointF> adjustedPoints;
    for (int i = 0; i < place.size(); ++i)
    {
        int x = chart->xAxs->pixelFromCoord(place[i].x());
        int y = chart->yAxs->pixelFromCoord(place[i].y());

        adjustedPoints.append(QPointF(x, y));
    }

    for (int i = 0; i < adjustedPoints.size(); ++i)
        painter->drawText(adjustedPoints.at(i), data.at(i));
}
