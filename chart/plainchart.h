#ifndef PLAINCHART_H
#define PLAINCHART_H

#include "chartdata.h"
#include "chartaxis.h"

#include <QLabel>

class ChartAxis;
class ChartText;
class ChartLayer;

class PlainChart : public QLabel
{
    Q_OBJECT

public:
    explicit PlainChart(QWidget *parent = 0);
    ~PlainChart();

    void replot();

    ChartDataItem* createDataItem(DataType type);
    void addTextItem(const QPointF& point, const QString& str);
    void rescaleAxes() { updateRanges(); }
    void setHeightItem(ChartDataItem* item) { data->setHeightItem(item); }

    ChartAxis* xAxis() const { return xAxs; }
    ChartAxis* yAxis() const { return yAxs; }
    void setExtremes(qreal x_min, qreal x_max, qreal y_min, qreal y_max);
    void setGridStep(qreal step_x, qreal step_y);
    void setAngles(bool enabled);
    void resetBounds();
    void resetStep() { recalcStep = true; }
    void clear();

protected:
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);

private:
    ChartAxis* xAxs;
    ChartAxis* yAxs;
    ChartData* data;
    ChartText* text;

    ChartLayer* axisLayer;
    ChartLayer* dataLayer;
    ChartLayer* textLayer;

    int textWidth;
    int textHeight;
    bool recalcBounds, recalcStep;

    void calcChartParams(QPainter* painter);

    void calcCoordsPoints(const QPoint &pointer);
    void calcCoordsAngle(const QPoint& pointer);

    void updateRanges();
    void updateSizeAspects();

signals:
    void currentAngle(qreal);
    void currentCoords(qreal, qreal);

    friend class ChartAxis;
    friend class ChartData;
    friend class ChartText;
};

#endif // PLAINCHART_H
