#ifndef CHARTLAYERITEM_H
#define CHARTLAYERITEM_H

#include <QWidget>
#include <QPen>

class ChartLayerItem
{
public:
    ChartLayerItem() {}
    virtual ~ChartLayerItem() {}

    virtual void paint(QPainter* painter) = 0;
};


#endif // CHARTLAYERITEM_H
