#include "chartaxis.h"
#include "chartlayer.h"
#include "chartdata.h"
#include "charttext.h"
#include "plainchart.h"
#include "qmath.h"

#include <QPainter>
#include <QTransform>
#include <QMouseEvent>


static inline qreal correct_ceil(qreal value, bool max)
{
    if (max)
        return qCeil(value);
    else
        return qFloor(value);
}

static inline qreal round_step(qreal step, qreal min_size)
{
    if (step > 1 && step < 10)
        return (int)step;
    if (step <= 1)
        return step;

    int i_step = step;

    const QString s_step = QString::number(i_step);
    const int f_pow = qPow(10, s_step.size() - 1);
    const int s_pow = qPow(10, s_step.size() - 2);

    while (true)
    {
        int s_num = i_step / s_pow % 10;

        if (s_num < 3)
            i_step -= s_num * s_pow;
        else if (s_num >= 3 && s_num < 8)
            i_step += (5 - s_num) * s_pow;
        else
        {
            i_step += f_pow;
            i_step -= s_num * s_pow;
        }

        i_step = i_step / s_pow * s_pow;

        if (i_step > min_size)
            return (qreal)i_step;
        else if (s_num == 5 || s_num == 0)
            i_step += 3 * s_pow;
    }

    return 0.0;
}


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
    delete xAxs;
    delete yAxs;
    delete data;
    delete text;
    delete axisLayer;
    delete dataLayer;
    delete textLayer;
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
    xAxs->setRange(correct_ceil(x_min, false), correct_ceil(x_max, true));
    yAxs->setRange(correct_ceil(y_min, false), correct_ceil(y_max, true));

    recalcBounds = false;
}

void PlainChart::setGridStep(qreal step_x, qreal step_y)
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
    const QString text = QString::number(-(int)qMax(qAbs(xAxs->min()), qAbs(xAxs->max())));
    const int text_width = metric.width(text);
    const int text_height = metric.height();
    const qreal coord_text_width = qAbs(xAxs->coordFromPixel(text_width + 10) - xAxs->coordFromPixel(0));
    const qreal coord_text_height = qAbs(yAxs->coordFromPixel(text_height + 2) - yAxs->coordFromPixel(0));

    textWidth = text_width;
    textHeight = text_height;

    if (recalcStep)
    {
        const int x_number_of_ticks = xAxs->numberOfTicks();
        const int y_number_of_ticks = yAxs->numberOfTicks();

        const qreal x_step = xAxs->getSpan() / x_number_of_ticks;
        const qreal y_step = yAxs->getSpan() / y_number_of_ticks;

        xAxs->setCell(round_step(x_step, coord_text_width));
        yAxs->setCell(round_step(y_step, coord_text_height));
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

    //    emit currentCoords(meter(0), meter(0));
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
    const qreal x = xAxs->coordFromPixel(pointer.x());
    const qreal y = yAxs->coordFromPixel(pointer.y());

    qreal resultX = x;
    qreal resultY = y;

    if (data->heightItem() != NULL)
        resultY = data->heightItem()->heightValue(resultX);
    else
        resultY = y;

    emit currentCoords(resultX, resultY);
}

void PlainChart::calcCoordsAngle(const QPoint& pointer)
{
    const qreal x = xAxs->coordFromPixel(pointer.x());
    const qreal y = yAxs->coordFromPixel(pointer.y());

    qreal angle = qAtan2(y, x);

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
