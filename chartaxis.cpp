#include "chartaxis.h"
#include "chartdata.h"
#include "charttext.h"
#include "chartlayeritem.h"
#include "plainchart.h"

#include <QPainter>

ChartGrid::ChartGrid()
    : zeroLinePen(QPen(Qt::gray, 0.8, Qt::DashDotDotLine)),
      gridPen(QPen(Qt::gray, 0.8, Qt::DashDotDotLine)),
      anglePen(QPen(Qt::darkGray, 0.8, Qt::DashDotDotLine)),
      drawAngle(false)
{
}

void ChartGrid::paint(QPainter* painter, const QVector<QPair<QPointF, QPointF> >& points)
{
    initPainter(painter);

    const QPair<QPointF, QPointF>& zeroLine = points.at(0);

    painter->setPen(zeroLinePen);
    painter->drawLine(zeroLine.first, zeroLine.second);

    painter->setPen(gridPen);
    for (int i = 1; i < points.size(); ++i)
    {
        const QPair<QPointF, QPointF> pair = points.at(i);
        painter->drawLine(pair.first, pair.second);
    }

    if (drawAngle)
    {
        painter->setPen(anglePen);
        const int max_y = static_cast<QWidget*>(painter->device())->height();

        for (int i = 15; i <= 75; i += 15)
        {
            const double x = max_y / qTan(i);

            painter->drawLine(0, max_y, x, 0);
        }
    }
}

void ChartGrid::initPainter(QPainter* painter)
{
    painter->resetTransform();
}


ChartAxis::ChartAxis(PlainChart* parent, bool is_horiz, bool is_invert)
    : QWidget(parent),
      ChartLayerItem(),
      ChartRange(),
      grd(new ChartGrid()),
      chart(parent),
      labelPen(QPen(Qt::gray)),
      centerPx(0), lbPos(0), cellSize(0),
      isHoriz(is_horiz), isInvert(is_invert), divide(false),
      offst(0), shft(0)
{
    if (is_horiz)
    {
        labelPos = Qt::AlignBottom;
        divideThreshold = 20000;
    }
    else
    {
        labelPos = Qt::AlignLeft;
        divideThreshold = 6000;
    }
}

void ChartAxis::setRange(double newS, double newF)
{
    if (isInvert)
        setRangeImpl(newF, newS);
    else
        setRangeImpl(newS, newF);
}

void ChartAxis::setAlignment(Qt::AlignmentFlag newPos)
{
    if (isHoriz)
    {
        if (newPos >= Qt::AlignTop && newPos <= Qt::AlignVCenter)
            labelPos = newPos;
    }
    else
    {
        if (newPos <= Qt::AlignHCenter)
            labelPos = newPos;
    }
}

qreal ChartAxis::coordFromPixel(int pixel) const
{
    const int sign = (isInvert) ? -1 : 1;

    return start() + sign * pixel * scale();
}

int ChartAxis::pixelFromCoord(qreal coord) const
{
    const int sign = (isInvert) ? -1 : 1;

    return sign * (coord - start()) / scale();
}

void ChartAxis::setSize(int newSize)
{
    setSizeImpl(newSize);
}

void ChartAxis::updateLabelPos()
{
    if (labelPos == Qt::AlignTop)
        lbPos = 0 + chart->textHeight;
    if (labelPos == Qt::AlignBottom)
        lbPos = chart->height();
    if (labelPos == Qt::AlignLeft)
        lbPos = 0;
    if (labelPos == Qt::AlignRight)
        lbPos = chart->width() - chart->textWidth;
    if (labelPos == Qt::AlignVCenter)
        lbPos = chart->height() / 2;
    if (labelPos == Qt::AlignHCenter)
        lbPos = chart->width() / 2; 
}

void ChartAxis::initPainter(QPainter* painter)
{
    painter->resetTransform();
    painter->setPen(labelPen);
}

bool ChartAxis::isDivided() const
{
    if (divide)
        return !(getSpan() <= divideThreshold);

    return false;
}

void ChartAxis::paint(QPainter* painter)
{
    const QTransform oldTr = painter->transform();
    const QRect oldWindow = painter->window();

    initPainter(painter);
    updateLabelPos();

    const int pos = lbPos;
    const bool drawDivided = !chart->data->isEmpty() && isDivided();
    const QVector<int>& points = calculatePoints();
    QVector<QPair<QPointF, QPointF> > gridPoints;

    for (int i = 0; i < points.size(); ++i)
    {
        const qreal coord = (points[i] == center()) ? 0 : coordFromPixel(points[i]);
        const QString text = QString::number(drawDivided ? qRound(coord / 1000) : coord, 'f',
                                             (coord == int(coord)) ? 0 : 2);

        QPointF point;

        if (isHoriz)
        {
            point = QPointF(points[i], pos);
            gridPoints.append(qMakePair(QPointF(points[i], 0), QPointF(points[i], chart->height())));
        }
        else
        {
            point = QPointF(pos, points[i]);
            gridPoints.append(qMakePair(QPointF(0, points[i]), QPointF(chart->width(), points[i])));
        }

        //тут можно добавить отрисовку тиков осей, если она будет нужна
        //

        chart->text->addAbsText(point, text);
    }

    grd->paint(painter, gridPoints);

    painter->setTransform(oldTr);
    painter->setWindow(oldWindow);
}

QVector<int> ChartAxis::calculatePoints()
{
    QVector<int> points;

    const int cell_size = cellSize;
    const int start = pixelFromCoord(0);
    const int finish = pixelSpan();

    //однозначное добавление оси
    points.append(start);

    for (int i = start + cell_size; i <= finish; i += cell_size)
        points.append(i);

    for (int i = start - cell_size; i >= 0; i -= cell_size)
        points.append(i);

    return points;
}


void ChartRange::setRangeImpl(double newS, double newF)
{
    st = newS; fn = newF;
    mn = std::min(newS, newF);
    mx = std::max(newS, newF);
    updateScale();
}
