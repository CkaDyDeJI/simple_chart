#include "chartdata.h"
#include "chartaxis.h"
#include "plainchart.h"

#include <algorithm>
#include <limits>

#include <QPainter>

static inline bool pointXCompare(QPointF first, QPointF second)
{
    return first.x() < second.x();
}

static inline bool pointYCompare(QPointF first, QPointF second)
{
    return first.y() < second.y();
}

static inline void calcBounds(QVector<qreal>& old_bounds, const QVector<qreal>& vec)
{
    old_bounds[0] = std::min(old_bounds[0], vec[0]);
    old_bounds[1] = std::max(old_bounds[1], vec[1]);
    old_bounds[2] = std::min(old_bounds[2], vec[2]);
    old_bounds[3] = std::max(old_bounds[3], vec[3]);
}

static inline void calcBounds(QVector<qreal>& bounds, const QVector<QPointF>& vec)
{
    bounds.clear();

#if __cplusplus > 199711L
    const auto x_pair = (std::minmax_element(vec.begin(), vec.end(), pointXCompare));
    const auto y_pair = (std::minmax_element(vec.begin(), vec.end(), pointYCompare));

    bounds << x_pair.first->x() << x_pair.second->x() << y_pair.first->y() << y_pair.second->y();
#else
    bounds << (std::min_element(vec.begin(), vec.end(), pointXCompare))->x();
    bounds << (std::max_element(vec.begin(), vec.end(), pointXCompare))->x();
    bounds << (std::min_element(vec.begin(), vec.end(), pointYCompare))->y();
    bounds << (std::max_element(vec.begin(), vec.end(), pointYCompare))->y();
#endif
}


ChartData::ChartData(PlainChart* chart)
    : ChartLayerItem(),
      chart(chart),
      hghtItem(NULL)
{

}

void ChartData::paint(QPainter* painter)
{
    const QTransform oldTr = painter->transform();
    const QRect oldWindow = painter->window();

    initPainter(painter);

    for (int i = 0; i < mData.size(); ++i)
        mData.at(i)->paint(painter);

    painter->setTransform(oldTr);
    painter->setWindow(oldWindow);
}

ChartDataItem* ChartData::createItem(DataType type)
{
    ChartDataItem* dataItem;

    if (type == polygs)
        dataItem = new ChartPolygonData();
    else if (type == trajects)
        dataItem = new ChartTrajectoryData();
    else if (type == spline)
        dataItem = new ChartSplineData();
    else
        dataItem = new ChartPointData();

    mData.append(dataItem);

    return dataItem;
}

void ChartData::addDataItem(ChartDataItem* item)
{
    mData.append(item);
}

ChartDataItem* ChartData::itemAt(int index)
{
    if (index >= mData.size())
        return NULL;

    return mData.at(index);
}

void ChartData::setHeightItem(ChartDataItem* item)
{
    hghtItem = static_cast<ChartSplineData*>(item);
}

void ChartData::clearData()
{
    hghtItem = NULL;

    for (int i = 0; i < mData.size(); ++i)
    {
        mData[i]->clearData();
        delete mData[i];
    }

    mData.clear();
}

bool ChartData::isEmpty() const
{
    return mData.isEmpty();
}

const QVector<qreal> ChartData::range() const
{
    QVector<qreal> bounds = QVector<qreal>()
                            << std::numeric_limits<qreal>::max() << std::numeric_limits<qreal>::min()
                            << std::numeric_limits<qreal>::max() << std::numeric_limits<qreal>::min();

    for (int i = 0; i < mData.size(); ++i)
        calcBounds(bounds, mData.at(i)->range());

    if (bounds.at(0) == bounds.at(1))
    {
        bounds[0] -= 2.5;
        bounds[1] += 2.5;
    }

    if (bounds.at(2) == bounds.at(3))
    {
        bounds[2] -= 2.5;
        bounds[3] += 2.5;
    }

    return bounds;
}

void ChartData::initPainter(QPainter* painter)
{
    const qreal x_offset = chart->xAxs->offset();
    const qreal y_offset = chart->yAxs->offset();
    const qreal x_span = chart->xAxs->getSpan();
    const qreal y_span = chart->yAxs->getSpan();
    const qreal x_shift = chart->xAxs->shift();
    const qreal y_shift = chart->yAxs->shift();
    const int x_scale = (chart->xAxs->isInverted()) ? -1 : 1;
    const int y_scale = (chart->yAxs->isInverted()) ? -1 : 1;

    const QTransform transform = QTransform().translate(x_offset , y_offset).scale(x_scale, y_scale);
    const QRect window(x_shift, y_shift, x_span, y_span);

    //перемещаем начало координат в нужную позицию и направляем оси вправо и вверх
    //задаем логическую систему координат
    painter->setTransform(transform);
    painter->setWindow(window);
}


ChartPolygonData::ChartPolygonData()
    : ChartDataItem()
{
    mainPen = QPen(Qt::blue, 1, Qt::DashLine);
    mainBrush = QBrush(Qt::blue, Qt::NoBrush);
}

void ChartPolygonData::paint(QPainter* painter)
{
    painter->setBrush(mainBrush);
    painter->setPen(mainPen);

    painter->drawPolygon(polygs, Qt::OddEvenFill);
}

void ChartPolygonData::setData(const QVector<QPointF>& pols)
{
    if (pols.size() == 0)
        return;

    QPolygonF polys = static_cast<QPolygonF>(pols);

    setPolys(polys);
    calcBounds(bounds, polys);
}

void ChartPolygonData::clearData()
{
    polygs.clear();
    bounds.clear();
    bounds.resize(4);
}

void ChartPolygonData::setPolys(const QPolygonF& pols)
{
    polygs = pols;
}


ChartTrajectoryData::ChartTrajectoryData()
    : ChartDataItem()
{
    mainPen = QPen(Qt::blue, 1, Qt::SolidLine);
    mainBrush = QBrush(Qt::blue);
}

void ChartTrajectoryData::paint(QPainter* painter)
{
    const qreal w_rect = (painter->window().width() / painter->viewport().width() == 0)
                             ? 1 : (double)painter->window().width() / painter->viewport().width() * 4;
    const qreal h_rect = (painter->window().height() / painter->viewport().height() == 0)
                             ? 1 : (double)painter->window().height() / painter->viewport().height() * 4;

    mainPen.setWidth(h_rect / 4);

    painter->setBrush(mainBrush);
    painter->setPen(mainPen);

    if (traj.size() == 1)
        painter->drawRect(QRectF(traj.at(0).x() - w_rect / 2, traj.at(0).y() - h_rect / 2, w_rect, h_rect));
    else
    {
        for (int j = 1; j < traj.size(); ++j)
            painter->drawLine(traj.at(j-1), traj.at(j));
    }
}

void ChartTrajectoryData::setData(const QVector<QPointF>& data)
{
    if (data.size() == 0)
        return;

    setTraj(data);
    calcBounds(bounds, data);
}

void ChartTrajectoryData::setColor(Qt::GlobalColor trajectoryColor)
{
    mainPen.setColor(trajectoryColor);
    mainBrush.setColor(trajectoryColor);
}

void ChartTrajectoryData::setTraj(const QVector<QPointF>& newTraj)
{
    traj = newTraj;
}

void ChartTrajectoryData::clearData()
{
    traj.clear();
    bounds.clear();
    bounds.resize(4);
}


ChartSplineData::ChartSplineData()
    : ChartDataItem()
{
    mainPen = QPen(Qt::darkGreen, 1, Qt::SolidLine);
    mainBrush = QBrush(Qt::green);
}

void ChartSplineData::paint(QPainter* painter)
{
    const int h_rect = (painter->window().height() / painter->viewport().height() == 0)
                           ? 1 : (double)painter->window().height() / painter->viewport().height() * 4;

    mainPen.setWidth(h_rect / 2);

    //рисуем профиль маршрута
    painter->setBrush(mainBrush);
    painter->setPen(mainPen);
    painter->drawConvexPolygon(spline);
}

void ChartSplineData::setSpline(const QVector<QPointF>& prof)
{
    spline = prof;
}

void ChartSplineData::setData(const QVector<QPointF>& prof)
{
    QVector<QPointF> profs = prof;

    if (profs.size() <= 1)
        return;

    profs.prepend(QPointF(prof.first().x(), 0));
    profs.append(QPointF(prof.last().x(), 0));

    setSpline(profs);
    calcBounds(bounds, prof);   //чтобы не учитывать новодобавленные точки
}

void ChartSplineData::clearData()
{
    spline.clear();
    bounds.clear();
    bounds.resize(4);
}

qreal ChartSplineData::heightValue(qreal x_value) const
{
    qreal result = 0.0;

    for (int i = 0; i < spline.size() - 1; ++i)
    {
        if (spline.at(i).x() <= x_value && spline.at(i+1).x() > x_value)
            result = spline.at(i).y();
    }

    return result;
}


ChartPointData::ChartPointData()
    : ChartDataItem(),
      zeroPointPen(QPen(Qt::green, 5, Qt::SolidLine)),
      zeroPointBr(Qt::green)
{
    mainPen = QPen(Qt::red, 5, Qt::SolidLine);
    mainBrush = QBrush(Qt::red);
}

void ChartPointData::paint(QPainter* painter)
{
    const qreal w_rect = (painter->window().width() / painter->viewport().width() == 0)
                           ? 1 : (double)painter->window().width() / painter->viewport().width() * 4;
    const qreal h_rect = (painter->window().height() / painter->viewport().height() == 0)
                           ? 1 : (double)painter->window().height() / painter->viewport().height() * 4;

    zeroPointPen.setWidth(h_rect / 4);
    mainPen.setWidth(h_rect / 4);

    //рисуем точки стояния БМ
    painter->setBrush(zeroPointBr);
    painter->setPen(zeroPointPen);
    if (!points.isEmpty())
        painter->drawRect(QRectF(points.at(0).x() - w_rect / 2, points.at(0).y() - h_rect / 2, w_rect, h_rect));

    painter->setBrush(mainBrush);
    painter->setPen(mainPen);
    for (int i = 1; i < points.size(); ++i)
        painter->drawRect(QRectF(points.at(i).x() - w_rect / 2, points.at(i).y() - h_rect / 2,
                                 w_rect, h_rect));
}

void ChartPointData::setPoints(const QVector<QPointF>& pts)
{
    points = pts;
}

void ChartPointData::setData(const QVector<QPointF>& points)
{
    if (points.size() == 0)
        return;

    setPoints(points);
    calcBounds(bounds, points);
}

void ChartPointData::clearData()
{
    points.clear();
    bounds.clear();
    bounds.resize(4);
}
