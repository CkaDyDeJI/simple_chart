#include "chartaxis.h"
#include "chartlayer.h"
#include "chartdata.h"
#include "charttext.h"
#include "plainchart.h"
#include "qmath.h"

#include <QPainter>
#include <QTransform>
#include <QMouseEvent>

PlainChart::PlainChart(QWidget *parent)
    : QLabel(parent),
      xAxs(new ChartAxis(this, true, false)),
      yAxs(new ChartAxis(this, false, true)),
      data(new ChartData(this)),
      text(new ChartText(this)),
      axisLayer(new ChartLayer()),
      dataLayer(new ChartLayer()),
      textLayer(new ChartLayer()),
      recalcBounds(true), recalcStep(true)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);

    axisLayer->addLayer(xAxs);
    axisLayer->addLayer(yAxs);
    dataLayer->addLayer(data);
    textLayer->addLayer(text);

    updateSizeAspects();
}

PlainChart::~PlainChart()
{
    clear();
}

void PlainChart::replot()
{
    setMouseTracking(true);
    updateRanges();
    update();
}

ChartDataItem* PlainChart::createDataItem(DataType type)
{
    return data->createItem(type);
}

void PlainChart::addTextItem(const QPointF& point, const QString& str)
{
    text->addText(point, str);
}

void PlainChart::setExtremes(qreal x_min, qreal x_max, qreal y_min, qreal y_max)
{
    xAxs->setRange(x_min, x_max);
    yAxs->setRange(y_min, y_max);

    recalcBounds = false;
}

void PlainChart::setGridStep(int step_x, int step_y)
{
    xAxs->setCell(step_x);
    yAxs->setCell(step_y);

    recalcStep = false;
}

void PlainChart::setAngles(bool enabled)
{
    xAxs->grid()->drawAngles(enabled);
}

void PlainChart::resetBounds()
{
    xAxs->setRange(0, 0);
    yAxs->setRange(0, 0);

    recalcBounds = true;
}

void PlainChart::calcChartParams(QPainter* painter)
{    
    const QFontMetrics& metric = painter->fontMetrics();
    const QString text = QString::number(xAxs->finish());
    const int text_width = metric.horizontalAdvance(text) + 10;

    textWidth = text_width - 10;
    textHeight = metric.height();

    if (recalcStep)
    {
        const int min_ax_size = std::min(width(), height()) / 5.0;

        int x_cell = 0, y_cell = 0;
        x_cell = y_cell = (min_ax_size > text_width) ? min_ax_size : text_width;

        //если шаг сетки слишком большой для оси y, то устанавливаем его по умолчанию (высота / 5)
        if (height() / y_cell <= 3)
            y_cell = height() / 5;

        xAxs->setCell(x_cell);
        yAxs->setCell(y_cell);
    }
}

void PlainChart::clear()
{
    setMouseTracking(false);

    data->clearData();
    text->clearData();
    text->clearAbsData();

    xAxs->setRange(0, 0);
    yAxs->setRange(0, 0);
    recalcBounds = true;
    recalcStep = true;
}

void PlainChart::resizeEvent(QResizeEvent *)
{
    updateSizeAspects();
    update();
}

void PlainChart::paintEvent(QPaintEvent *)
{
    if (data->isEmpty())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    calcChartParams(&painter);
    text->clearAbsData();

    dataLayer->paint(&painter);
    axisLayer->paint(&painter);
    textLayer->paint(&painter);
}

void PlainChart::mouseMoveEvent(QMouseEvent *event)
{
    if (data->isEmpty())
        return;

    const QPoint pointer = event->pos();

    calcCoordsPoints(pointer);
    calcCoordsAngle(pointer);
}

void PlainChart::calcCoordsPoints(const QPoint &pointer)
{
    qreal x = xAxs->coordFromPixel(pointer.x());
    qreal y = yAxs->coordFromPixel(pointer.y());

    if (data->heightItem() != NULL)
        y = data->heightItem()->heightValue(x);

    emit currentCoords(x, y);
}

void PlainChart::calcCoordsAngle(const QPoint& pointer)
{
    const qreal x = xAxs->coordFromPixel(pointer.x());
    const qreal y = yAxs->coordFromPixel(pointer.y());

    double angle = qAtan2(y, x);
//    degree angle = rad2deg(radian(qAtan2(y, x)));

    emit currentAngle(angle);
}

void PlainChart::updateRanges()
{
    qreal x_start = 0,  x_finish = 0;
    qreal y_start = 0,  y_finish = 0;
    qreal x_offset = 0, y_offset = 0;
    qreal x_shift = 0,  y_shift = 0;

    const int x_sign = xAxs->isInverted() ? -1 : 1;
    const int y_sign = yAxs->isInverted() ? -1 : 1;

    if (recalcBounds)
    {
        const QVector<qreal>& bounds = data->range();

        if (!bounds.isEmpty())
        {
            x_start = bounds.at(0);
            x_finish = bounds.at(1);
            y_start = bounds.at(2);
            y_finish = bounds.at(3);
        }
    }
    else
    {
        x_start = xAxs->min();
        x_finish = xAxs->max();
        y_start = yAxs->min();
        y_finish = yAxs->max();
    }

    x_offset = qAbs(x_finish - x_start);
    y_offset = qAbs(y_finish - y_start);

    const qreal x_local = xAxs->isInverted() ? x_finish : x_start;
    const qreal y_local = yAxs->isInverted() ? y_finish : y_start;

    x_shift = x_sign * (x_local + x_sign * x_offset);
    y_shift = y_sign * (y_local + y_sign * y_offset);

    xAxs->setRange(x_start, x_finish);
    yAxs->setRange(y_start, y_finish);

    xAxs->setOffset(x_offset);
    yAxs->setOffset(y_offset);

    xAxs->setShift(x_shift);
    yAxs->setShift(y_shift);
}

void PlainChart::updateSizeAspects()
{
    xAxs->setSize(width());
    yAxs->setSize(height());
}
