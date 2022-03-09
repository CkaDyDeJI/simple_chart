#include "chartlayer.h"
#include "chartlayeritem.h"


ChartLayer::ChartLayer()
{

}

void ChartLayer::addLayer(ChartLayerItem* newLayer)
{
    layers.append(newLayer);
}

void ChartLayer::removeLayer(ChartLayerItem* layer)
{
    layers.remove(layers.indexOf(layer));
}

void ChartLayer::paint(QPainter* painter)
{
    foreach (ChartLayerItem* item, layers)
        item->paint(painter);
}
