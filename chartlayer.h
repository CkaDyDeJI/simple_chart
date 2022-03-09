#ifndef CHARTLAYER_H
#define CHARTLAYER_H

#include <QWidget>

class ChartLayerItem;

class ChartLayer
{
public:
    ChartLayer();

    void addLayer(ChartLayerItem* newLayer);
    void removeLayer(ChartLayerItem* layer);

    void paint(QPainter* painter);

private:
    QVector<ChartLayerItem*> layers;
};

#endif // CHARTLAYER_H
