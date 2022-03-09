#ifndef CHARTAXIS_H
#define CHARTAXIS_H

#include "chartlayeritem.h"

#include <QWidget>
#include <QPen>

class PlainChart;

class ChartRange
{
public:
    ChartRange() { }
    explicit ChartRange(double newStart, double newFinish) : st(newStart), fn(newFinish) { }

    double getSpan() const { return qAbs(fn - st); }
    double start() const { return st; }
    double finish() const { return fn; }
    double min() const { return mn; }
    double max() const { return mx; }
    qreal scale() const { return scl; }
    int pixelSpan() const { return sz; }

protected:
    void setRangeImpl(double newS, double newF);
    void setSizeImpl(int newSize) { sz = newSize; updateScale(); }

private:
    void updateScale() { scl = getSpan() / sz; }

    double st;
    double fn;
    double mn, mx;

    int sz;
    qreal scl;
};


class ChartGrid
{
public:
    explicit ChartGrid();

    void paint(QPainter* painter, const QVector<QPair<QPointF, QPointF> >& points);

    void setZeroLinePen(QPen newPen) { zeroLinePen = newPen; }
    void setGridPen(QPen newPen) { gridPen = newPen; }
    void setAnglePen(QPen newPen) { anglePen = newPen; }
    void drawAngles(bool enabled) { drawAngle = enabled; }

private:
    void initPainter(QPainter* painter);

    QPen zeroLinePen;
    QPen gridPen;
    QPen anglePen;
    bool drawAngle;
};


class ChartAxis : public QWidget, public ChartLayerItem, public ChartRange
{
    Q_OBJECT
public:
    explicit ChartAxis(PlainChart* parent = NULL, bool is_horiz = false, bool is_invert = false);

    virtual void paint(QPainter* painter);

    void setRange(double newS, double newF);
    void setAlignment(Qt::AlignmentFlag newPos);
    void setCenter(int newCenter) { centerPx = newCenter; }
    void setSize(int newSize);
    void setOffset(qreal newOffset) { offst = newOffset; }
    void setShift(qreal newShift) { shft = newShift; }
    void setPen(QPen newPen) { labelPen = newPen; }
    void setCell(int newCell) { cellSize = newCell; }
    void setDivideThreshold(int newTr) { divideThreshold = newTr; }
    void setDivideLabel(bool enabled) { divide = enabled; }

    ChartGrid* grid() const { return grd; }
    int center() const { return centerPx; }
    int cell() const { return cellSize; }
    qreal offset() const { return offst; }
    qreal shift() const { return shft; }
    bool isInverted() const { return isInvert; }

    qreal coordFromPixel(int pixel) const;
    int pixelFromCoord(qreal coord) const;

private:
    void initPainter(QPainter* painter);
    int pixelFromAbsPixel(int pixel) const;
    bool isDivided() const;
    void updateLabelPos();

    QVector<int> calculatePoints();

    ChartGrid* grd;
    PlainChart* chart;
    QPen labelPen;
    Qt::AlignmentFlag labelPos;
    int centerPx, lbPos, cellSize, divideThreshold;
    bool isHoriz, isInvert, divide;
    qreal offst, shft;
};

#endif // CHARTAXIS_H
