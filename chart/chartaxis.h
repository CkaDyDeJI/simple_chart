#ifndef CHARTAXIS_H
#define CHARTAXIS_H

#include "chartlayeritem.h"

#include <QWidget>

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


class ChartAxis : public ChartLayerItem, public ChartRange
{
public:
    explicit ChartAxis(PlainChart* parent = NULL, bool is_horiz = false, bool is_invert = false);
    virtual ~ChartAxis();

    virtual void paint(QPainter* painter);

    void setRange(double newS, double newF);
    void setAlignment(Qt::AlignmentFlag newPos);
    void setSize(int newSize);
    void setOffset(qreal newOffset) { offst = newOffset; }
    void setShift(qreal newShift) { shft = newShift; }
    void setPen(QPen newPen) { labelPen = newPen; }
    void setCell(qreal newCell) { cellSize = newCell; }
    void setDivideThreshold(int newTr) { divideThreshold = newTr; }
    void setDivideLabel(bool enabled) { divide = enabled; }
    void setNumberOfTicks(int newT) { numOfTicks = newT; }
    void setNumberOfSubTicks(int newT) { numOfSubTicks = newT; }

    ChartGrid* grid() const { return grd; }
    int cell() const { return cellSize; }
    qreal offset() const { return offst; }
    qreal shift() const { return shft; }
    bool isInverted() const { return isInvert; }
    int numberOfTicks() const { return numOfTicks; }

    qreal coordFromPixel(int pixel) const;
    int pixelFromCoord(qreal coord) const;

private:
    void initPainter(QPainter* painter);
    bool isDivided() const;
    void updateLabelPos();

    QVector<qreal> calculatePoints();

    ChartGrid* grd;
    PlainChart* chart;
    QPen labelPen;
    Qt::AlignmentFlag labelPos;
    int numOfTicks, numOfSubTicks;
    int lbPos, divideThreshold, prev;
    bool isHoriz, isInvert, divide;
    qreal offst, shft, cellSize;
};

#endif // CHARTAXIS_H
